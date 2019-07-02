<template>
  <uci-section type="zone" :title="$t('Zones')" table addable :add="addZone" :before-del="beforeDelZone">
    <uci-option-dummy :label="$t('Name')" name="name"></uci-option-dummy>
    <uci-option-list :label="$t('Input')" name="input" :options="targets" initial="ACCEPT" required></uci-option-list>
    <uci-option-list :label="$t('Output')" name="output" :options="targets" initial="ACCEPT" required></uci-option-list>
    <uci-option-list :label="$t('Forward')" name="forward" :options="targets" initial="REJECT" required></uci-option-list>
    <uci-option-switch :label="$t('Masquerading')" name="masq"></uci-option-switch>
    <uci-option-switch :label="$t('MSS clamping')" name="mtu_fix"></uci-option-switch>
    <uci-table-expand>
      <uci-option-list :label="$t('Covered networks')" name="network" :options="interfaces" multiple></uci-option-list>
      <uci-option-list :label="$t('Restrict to address family')" name="family" :options="families"></uci-option-list>
      <uci-option-dlist :label="$t('Restrict Masquerading to given source subnets')" name="masq_src" :options="families" depend="family == '' || family == 'ipv4'"></uci-option-dlist>
      <uci-option-dlist :label="$t('Restrict Masquerading to given destination subnets')" name="masq_dest" :options="families" depend="family == '' || family == 'ipv4'"></uci-option-dlist>
      <uci-option-switch :label="$t('Force connection tracking')" name="conntrack"></uci-option-switch>
      <uci-option-switch :label="$t('Enable logging on this zone')" name="log"></uci-option-switch>
      <uci-option-input :label="$t('Limit log messages')" name="log_limit" placeholder="10/minute" depend="log"></uci-option-input>
      <uci-option-list :label="$t('Allow forward to destination zones')" name="out" :load="loadDestZones" :save="saveDestZones" :options="zones" :exclude="excludeZone" multiple></uci-option-list>
      <uci-option-list :label="$t('Allow forward from source zones')" name="in" :load="loadSrcZones" :save="saveSrcZones" :options="zones" :exclude="excludeZone" multiple></uci-option-list>
    </uci-table-expand>
  </uci-section>
</template>

<script>
export default {
  props: {
    zones: Array,
    targets: Array
  },
  data() {
    return {
      interfaces: [],
      families:[
        ['', this.$t('IPv4 and IPv6')],
        ['ipv4', this.$t('IPv4 only')],
        ['ipv6', this.$t('IPv6 only')]
      ]
    }
  },
  methods: {
    loadDestZones(sid) {
      const z = this.$firewall.findZoneBySid(sid);
      return z.findForwardsBy('src').map(z => z.dest());
    },
    loadSrcZones(sid) {
      const z = this.$firewall.findZoneBySid(sid);
      return z.findForwardsBy('dest').map(z => z.src());
    },
    excludeZone(sid) {
      const z = this.$firewall.findZoneBySid(sid);
      if (z)
        return z.name();
      return undefined;
    },
    saveDestZones(sid, value) {
      const z = this.$firewall.findZoneBySid(sid);
      const dests = value;
      let i = 0;

      this.$firewall.forwards.forEach(fwd => {
        if (fwd.src() !== z.name())
          return;

        if (i < dests.length)
          fwd.set('dest', dests[i++]);
        else
          this.$uci.del('firewall', fwd.sid);
      });

      while (i < dests.length) {
        const sid = this.$uci.add('firewall', 'forwarding');

        this.$uci.set('firewall', sid, 'src', z.name());
        this.$uci.set('firewall', sid, 'dest', dests[i++]);
      }
    },
    saveSrcZones(sid, value) {
      const z = this.$firewall.findZoneBySid(sid);
      const srcs = value;
      let i = 0;

      this.$firewall.forwards.forEach(fwd => {
        if (fwd.dest() !== z.name())
          return;

        if (i < srcs.length)
          fwd.set('src', srcs[i++]);
        else
          this.$uci.del('firewall', fwd.sid);
      });

      while (i < srcs.length) {
        const sid = this.$uci.add('firewall', 'forwarding');

        this.$uci.set('firewall', sid, 'src', srcs[i++]);
        this.$uci.set('firewall', sid, 'dest', z.name());
      }
    },
    beforeDelZone(sid) {
      const z = this.$firewall.findZoneBySid(sid);
      this.$firewall.forwards.forEach(fwd => {
        if (fwd.src() !== z.name() && fwd.dest() !== z.name())
          return;
        this.$uci.del('firewall', fwd.sid);
      });
    },
    addZone(self) {
      return new Promise(resolve => {
        this.$prompt(this.$t('Please input a name'), this.$t('Add'), {
          inputValidator: value => {
            if (!value)
              return true;

            const sections = self.sections;
            for (let i = 0; i < sections.length; i++)
              if (sections[i].name === value)
                return this.$t('Name already used');

            return true;
          }
        }).then(r => {
          if (!r.value)
            return;

          const z = this.$firewall.createZone(r.value);
          resolve(z.sid);
        });
      });
    }
  },
  created() {
    this.$network.load().then(() => {
      this.interfaces = this.$network.getInterfaces().map(item => item.name);
    });
  }
}
</script>
