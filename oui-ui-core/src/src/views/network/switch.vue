<template>
  <uci-form config="network" v-if="loaded">
    <template v-for="s in switchs">
      <uci-section :key="s.name" :title="switchTitle(s)" :name="s.sid">
        <uci-option v-if="s.attrs['enable_vlan']" type="switch" :label="$t('Enable VLAN functionality')" name="enable_vlan"></uci-option>
        <uci-option v-if="s.attrs['enable_learning']" type="switch" :label="$t('Enable learning and aging')" name="enable_learning"></uci-option>
      </uci-section>
      <uci-section :key="s.name + '_vlan'" :title="vlanTitle(s)" type="switch_vlan" :filter="filterVlanSection" table addable :add="addVlanSection" :option="{swname: s.name, num_vlans: s.num_vlans, max_vid: s.max_vid}">
        <uci-option type="input" label="VLAN ID" name="vlan" :rules="vidValidator" required></uci-option>
        <uci-option v-for="(port, i) in s.ports" :key="i" type="list" :label="portLabel(i, port)" :name="'port' + i" :options="switchPortState" initial="n" required :load="portLoad" :save="savePort"></uci-option>
      </uci-section>
    </template>
  </uci-form>
</template>

<script>
export default {
  data() {
    return {
      loaded: false,
      switchs: [],
      switchPortState: [
        ['n', this.$t('Switch port state - off')],
        ['u', this.$t('Switch port state - untagged')],
        ['t', this.$t('Switch port state - tagged')]
      ]
    }
  },
  methods: {
    switchTitle(info) {
      return this.$t('Switch') + `"${info.name}"(${info.model})`;
    },
    vlanTitle(info) {
      return this.$t('VLANs on-', {name: `"${info.name}"(${info.model})`});
    },
    portLabel(n, info) {
      let label = 'Port' + n;
      if (info.link) {
        label += `(${info.speed}baseT`;

        if (info.full_duplex)
          label += ' Full-duplex)';
        else
          label += ' Half-duplex)';
      } else {
        label += '(No link)';
      }
      return label;
    },
    filterVlanSection(vm, s) {
      return vm.option.swname === s.device;
    },
    addVlanSection(vm) {
      const usedVID = {};

      vm.uciSections.forEach(s => {
        if (s.vlan)
          usedVID[s.vlan] = true;
      });

      for (let i = 1; i < vm.option.num_vlans; i++) {
        if (usedVID[i.toString()])
          continue;
        const sid = this.$uci.add('network', 'switch_vlan');
        this.$uci.set('network', sid, 'device', vm.option.swname);
        this.$uci.set('network', sid, 'vlan', i.toString());
        return sid;
      }
    },
    vidValidator(val, vm) {
      const sections = vm.uciSection.uciSections;
      const usedVID = {};

      if (!val)
        return;

      for (let i = 0; i < sections.length; i++) {
        const sid = sections[i]['.name'];
        const v = vm.formValue(sid);
        if (!v)
          continue;
        if (usedVID[v])
          return this.$t('VLAN ID must be unique');
        usedVID[v] = true;
      }

      const max = vm.uciSection.option.max_vid;
      if (!val.match(/[^0-9]/)) {
        val = parseInt(val);
        if (val >= 1 && val <= max)
          return;
      }

      return this.$t('VID-ERR-MSG', {max: max});
    },
    portLoad(resolve, sid, name) {
      let ports = this.$uci.get('network', sid, 'ports') || '';
      ports = ports.split(' ');
      const id = name.substr(4);
      let v = 'n';

      if (ports.indexOf(id + 't') > -1)
        v = 't';

      if (ports.indexOf(id) > -1)
        v = 'u';

      resolve(v);
    },
    savePort(config, sid, name, val) {
      const ports = this.$uci.get('network', sid, 'ports').split(' ');
      const id = name.substr(4);

      let i = ports.indexOf(id);
      if (i === -1)
        i = ports.indexOf(id + 't');

      if (i !== -1)
        ports.splice(i, 1);

      if (val === 'u')
        ports.push(id);
      else if (val === 't')
        ports.push(id + 't');

      this.$uci.set(config, sid, 'ports', ports.join(' '));

      return false;
    }
  },
  created() {
    this.$uci.load('network').then(() => {
      const sections = this.$uci.sections('network', 'switch');
      let batch = [];

      sections.forEach(s => {
        batch.push(['oui.network', 'switch_info', {switch: s.name}]);
      });

      this.$ubus.callBatch(batch).then(rs => {
        if (!Array.isArray(rs))
          rs = [rs];

        rs.forEach((r, i) => {
          const info = r.info;
          const attrs = {};

          info.switch.forEach(attr => {
            attrs[attr.name] = true;
          });

          let max_vid = info.num_vlans - 1;
          const vlanAttrs = info.vlan.map(v => v.name);
          if (vlanAttrs.indexOf('tag') > -1 || vlanAttrs.indexOf('vid') > -1 || vlanAttrs.indexOf('pvid') > -1)
            max_vid = 4094;

          this.switchs.push(Object.assign({
            name: sections[i].name,
            sid: sections[i]['.name'],
            max_vid: max_vid,
            attrs: attrs
          }, info));
        });


        let batch = [];
        this.switchs.forEach(s => {
          batch.push(['oui.network', 'switch_status', {switch: s.name}]);
        });

        this.$ubus.callBatch(batch).then(rs => {
          if (!Array.isArray(rs))
            rs = [rs];

          rs.forEach((r, i) => {
            this.switchs[i].ports = r.ports;
          });
          this.loaded = true;
        });
      });
    });
  }
}
</script>
