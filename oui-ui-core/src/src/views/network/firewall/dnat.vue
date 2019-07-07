<template>
  <uci-section type="redirect" :title="$t('Port Forwards')" table addable sortable :filter="filterDnat" :after-add="afterAdd">
    <uci-option-input :label="$t('Name')" name="name"></uci-option-input>
    <uci-option-list :label="$t('Protocol')" name="proto" :options="protos" initial="tcp udp" allow-create></uci-option-list>
    <uci-option-list :label="$t('Source zone')" name="src" :options="zones" required></uci-option-list>
    <uci-option-input :label="$t('External port')" name="src_dport" rules="port"></uci-option-input>
    <uci-option-list :label="$t('Internal zone')" name="dest" :options="zones" required></uci-option-list>
    <uci-option-list :label="$t('Internal IP address')" name="dest_ip" :options="arps" allow-create rules="ip4addr"></uci-option-list>
    <uci-option-input :label="$t('Internal port')" name="dest_port" rules="port"></uci-option-input>
    <uci-option-switch :label="$t('Enable')" name="enabled" initial="1"></uci-option-switch>
    <uci-table-expand>
      <uci-option-dlist :label="$t('Source MAC address')" name="src_mac" rules="macaddr"></uci-option-dlist>
      <uci-option-input :label="$t('Source IP address')" name="src_ip" rules="ip4addr" :placeholder="$t('any')"></uci-option-input>
      <uci-option-input :label="$t('Source port')" name="src_port" rules="port" :placeholder="$t('any')" depend="proto == 'tcp' || proto == 'udp' || proto == 'tcp udp'"></uci-option-input>
      <uci-option-input :label="$t('External IP address')" name="src_dip" rules="ip4addr" :placeholder="$t('any')"></uci-option-input>
      <uci-option-switch :label="$t('Enable NAT Loopback')" name="reflection" initial="1"></uci-option-switch>
    </uci-table-expand>
  </uci-section>
</template>

<script>
export default {
  props: {
    zones: Array,
    protos: Array
  },
  data() {
    return {
      arps: []
    }
  },
  methods: {
    filterDnat(s) {
      return s.target !== 'SNAT';
    },
    afterAdd(sid) {
      this.$uci.set('firewall', sid, 'target', 'DNAT');
    }
  },
  created() {
    this.$ubus.call('oui.network', 'arp_table').then(r => {
      r.entries.forEach(arp => {
        if (arp.macaddr === '00:00:00:00:00:00')
          return;

        this.arps.push([arp.ipaddr, `${arp.ipaddr} (${arp.macaddr})`]);
      });
    });
  }
}
</script>
