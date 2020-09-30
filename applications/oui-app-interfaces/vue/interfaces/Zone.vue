<template>
  <oui-form-item-select :uci-section="uciSection" :label="$t('interfaces.Create / Assign firewall-zone')" name="_fwzone" :options="zones" :load="loadZone" :save="saveZone" allow-create :help="$t('interface-config-zone-desc')"/>
</template>

<script>
export default {
  name: 'Zone',
  props: {
    uciSection: Object
  },
  data () {
    return {
      zones: []
    }
  },
  methods: {
    loadZone () {
      return new Promise(resolve => {
        this.$firewall.load().then(() => {
          this.zones = this.$firewall.zones.map(z => z.name())
          const z = this.$firewall.findZoneByNetwork(this.editorIface)
          if (z) { resolve(z.name()) }
          resolve()
        })
      })
    },
    saveZone (self) {
      const ifname = this.uciSection['.name']
      const value = self.model
      let z = this.$firewall.findZoneByNetwork(ifname)

      if (!value) {
        if (z) { z.delNetwork(ifname) }
        return
      }

      if (z) {
        if (value === z.name()) { return }
        z.delNetwork(ifname)
      }

      z = this.$firewall.findZoneByName(value)
      if (!z) { z = this.$firewall.createZone(value) }
      z.addNetwork(this.editorIface)
    }
  }
}
</script>
