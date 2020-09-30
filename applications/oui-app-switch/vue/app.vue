<template>
  <oui-form uci-config="network">
    <a-tabs v-if="switchs.length > 0" :value="switchs[0].name" :animated="false">
      <a-tab-pane v-for="sw in switchs" :key="sw.name" :name="sw.name" :tab="switchTitle(sw)">
        <oui-named-section :name="sw.sid" v-slot="{ s }">
          <oui-form-item-switch v-if="sw.attrs['enable_vlan']" :uci-section="s" :label="$t('switch.Enable VLAN functionality')" name="enable_vlan"/>
          <oui-form-item-switch v-if="sw.attrs['enable_learning']" :uci-section="s" :label="$t('switch.Enable learning and aging')" name="enable_learning"/>
        </oui-named-section>
        <oui-typed-section title="VLAN" type="switch_vlan" :filter="s => s.device === sw.name" :columns="columns(sw.ports)" addremove :add="self => add(sw.name, self)">
          <template v-for="(p, i) in sw.ports" v-slot:[columnTitleSlot(i)]>
            <div :key="i" style="text-align: center">
              <div>{{ $t('switch.Port') + i }}</div>
              <template v-if="p.link">
                <img src="/icons/port_up.png"/>
                <div>{{ p.speed + 'baseT ' + (p.full_duplex ? $t('switch.Full-duplex') : $t('switch.Half-duplex')) }}</div>
              </template>
              <template v-else>
                <img src="/icons/port_down.png"/>
                <div>{{ $t('switch.No link') }}</div>
              </template>
            </div>
          </template>
          <template #vlan="{ s }">
            <oui-form-item-input :uci-section="s" name="vlan" rules="uinteger" required/>
          </template>
          <template v-for="i in sw.ports.length" v-slot:[columnSlot(i)]="{ s }">
            <oui-form-item-select :key="i" :uci-section="s" :name="'port' + (i - 1)" :options="switchPortState" initial="n" required :load="loadPort" :save="savePort"/>
          </template>
        </oui-typed-section>
      </a-tab-pane>
    </a-tabs>
  </oui-form>
</template>

<script>
export default {
  data () {
    return {
      switchs: [],
      switchPortState: [
        ['n', this.$t('switch.Off')],
        ['u', this.$t('switch.Untagged')],
        ['t', this.$t('switch.Tagged')]
      ]
    }
  },
  methods: {
    columns (ports) {
      return [{ name: 'vlan', label: 'VLAN ID', width: 300 }, ...ports.map((p, i) => {
        return {
          name: `port${i}`,
          width: 300,
          titleSlot: 'title' + i
        }
      })]
    },
    columnTitleSlot (i) {
      return 'title' + i
    },
    columnSlot (i) {
      return 'port' + (i - 1)
    },
    switchTitle (info) {
      return this.$t('Switch') + `"${info.name}"(${info.model})`
    },
    loadPort (self) {
      let ports = this.$uci.get('network', self.sid, 'ports') || ''
      ports = ports.split(' ')
      const id = self.name.substr(4)
      let v = 'n'

      if (ports.indexOf(id + 't') > -1) { v = 't' }

      if (ports.indexOf(id) > -1) { v = 'u' }

      return v
    },
    savePort (self) {
      const ports = this.$uci.get('network', self.sid, 'ports').split(' ')
      const id = self.name.substr(4)

      let i = ports.indexOf(id)
      if (i === -1) { i = ports.indexOf(id + 't') }

      if (i !== -1) { ports.splice(i, 1) }

      if (self.model === 'u') { ports.push(id) } else if (self.model === 't') { ports.push(id + 't') }

      this.$uci.set('network', self.sid, 'ports', ports.join(' '))
    },
    add (device, self) {
      const sid = self.addSection()
      this.$uci.set('network', sid, 'device', device)
      return sid
    }
  },
  created () {
    this.$uci.load('network').then(() => {
      const sections = this.$uci.sections('network', 'switch')
      sections.forEach(s => {
        const promises = []
        promises.push(this.$rpc.call('network', 'switch_info', { switch: s.name }))
        promises.push(this.$rpc.call('network', 'switch_status', { switch: s.name }))

        Promise.all(promises).then(rs => {
          const info = rs[0].info
          const ports = rs[1].ports
          const attrs = {}

          info.switch.forEach(attr => {
            attrs[attr.name] = true
          })

          let maxVid = info.num_vlans - 1
          const vlanAttrs = info.vlan.map(v => v.name)
          if (vlanAttrs.indexOf('tag') > -1 || vlanAttrs.indexOf('vid') > -1 || vlanAttrs.indexOf('pvid') > -1) { maxVid = 4094 }

          this.switchs.push(Object.assign({
            name: s.name,
            sid: s['.name'],
            max_vid: maxVid,
            attrs: attrs,
            ports: ports
          }, info))
        })
      })
    })
  }
}
</script>
