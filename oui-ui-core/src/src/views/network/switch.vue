<template>
  <el-tabs v-if="switchs.length > 0" :value="switchs[0].name">
    <el-tab-pane v-for="s in switchs" :key="s.name" :name="s.name" :label="switchTitle(s)">
      <uci-form config="network" :apply-timeout="15">
        <uci-section :name="s.sid">
          <uci-option-switch v-if="s.attrs['enable_vlan']" :label="$t('Enable VLAN functionality')" name="enable_vlan"></uci-option-switch>
          <uci-option-switch v-if="s.attrs['enable_learning']" :label="$t('Enable learning and aging')" name="enable_learning"></uci-option-switch>
        </uci-section>
        <uci-section title="VLAN" type="switch_vlan" :filter="filterVlanSection" table addable :add="addVlanSection" :options="{swname: s.name, num_vlans: s.num_vlans, max_vid: s.max_vid}">
          <uci-option-input label="VLAN ID" name="vlan" :rules="vidValidator" required></uci-option-input>
          <uci-option-list v-for="(port, i) in s.ports" :key="i" :header="portLabel(i, port)" :name="'port' + i" :options="switchPortState" initial="n" required :load="loadPort" :save="savePort"></uci-option-list>
        </uci-section>
      </uci-form>
    </el-tab-pane>
  </el-tabs>
</template>

<script>
export default {
  data() {
    return {
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
      let label = `<span>Port ${n}</span><br/>`;

      if (info.link) {
        label += '<img src="/icons/port_up.png"/><br/>'
        label += '<span>' + info.speed + 'baseT ';
        if (info.full_duplex)
          label += this.$t('Full-duplex');
        else
          label += this.$t('Half-duplex');
      } else {
        label += '<img src="/icons/port_down.png"/><br/>'
        label += '<span>' + this.$t('No link');
      }
      label += '</span>'
      return label;
    },
    filterVlanSection(s, self) {
      return self.options.swname === s.device;
    },
    addVlanSection(self) {
      const usedVID = {};

      self.uciSections.forEach(s => {
        if (s.vlan)
          usedVID[s.vlan] = true;
      });

      for (let i = 1; i < self.options.num_vlans; i++) {
        if (usedVID[i.toString()])
          continue;
        const sid = this.$uci.add('network', 'switch_vlan');
        this.$uci.set('network', sid, 'device', self.options.swname);
        this.$uci.set('network', sid, 'vlan', i.toString());
        return sid;
      }
    },
    vidValidator(val, self) {
      const sections = self.uciSection.uciSections;
      const usedVID = {};

      if (!val)
        return;

      for (let i = 0; i < sections.length; i++) {
        const sid = sections[i]['.name'];
        const v = self.formValue(sid);
        if (!v)
          continue;
        if (usedVID[v])
          return this.$t('VLAN ID must be unique');
        usedVID[v] = true;
      }

      const max = self.uciSection.options.max_vid;
      if (!val.match(/[^0-9]/)) {
        val = parseInt(val);
        if (val >= 1 && val <= max)
          return;
      }

      return this.$t('VID-ERR-MSG', {max: max});
    },
    loadPort(sid, self) {
      let ports = this.$uci.get('network', sid, 'ports') || '';
      ports = ports.split(' ');
      const id = self.name.substr(4);
      let v = 'n';

      if (ports.indexOf(id + 't') > -1)
        v = 't';

      if (ports.indexOf(id) > -1)
        v = 'u';

      return v;
    },
    savePort(sid, val, self) {
      const ports = this.$uci.get('network', sid, 'ports').split(' ');
      const id = self.name.substr(4);

      let i = ports.indexOf(id);
      if (i === -1)
        i = ports.indexOf(id + 't');

      if (i !== -1)
        ports.splice(i, 1);

      if (val === 'u')
        ports.push(id);
      else if (val === 't')
        ports.push(id + 't');

      this.$uci.set('network', sid, 'ports', ports.join(' '));
    }
  },
  created() {
    this.$uci.load('network').then(() => {
      const sections = this.$uci.sections('network', 'switch');
      sections.forEach(s => {
        let batch = [];
        batch.push(['oui.network', 'switch_info', {switch: s.name}]);
        batch.push(['oui.network', 'switch_status', {switch: s.name}]);

        this.$ubus.callBatch(batch).then(rs => {
          const info = rs[0].info;
          const ports = rs[1].ports;
          const attrs = {};

          info.switch.forEach(attr => {
            attrs[attr.name] = true;
          });

          let max_vid = info.num_vlans - 1;
          const vlanAttrs = info.vlan.map(v => v.name);
          if (vlanAttrs.indexOf('tag') > -1 || vlanAttrs.indexOf('vid') > -1 || vlanAttrs.indexOf('pvid') > -1)
            max_vid = 4094;

          this.switchs.push(Object.assign({
            name: s.name,
            sid: s['.name'],
            max_vid: max_vid,
            attrs: attrs,
            ports: ports
          }, info));
        });
      });
    });
  }
}
</script>
