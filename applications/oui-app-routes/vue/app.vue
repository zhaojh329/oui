<template>
  <oui-form uci-config="network">
    <oui-typed-section :title="$t('routes.Static IPv4 Routes')" type="route" :columns="routeColumns" addremove>
      <template #interface="{ s }">
        <oui-form-item-select :uci-section="s" name="interface" :options="interfaces" required/>
      </template>
      <template #target="{ s }">
        <oui-form-item-input :uci-section="s" name="target" required rules="ip4addr"/>
      </template>
      <template #netmask="{ s }">
        <oui-form-item-input :uci-section="s" name="netmask" placeholder="255.255.255.255" required rules="netmask4"/>
      </template>
      <template #gateway="{ s }">
        <oui-form-item-input :uci-section="s" name="gateway" rules="ip4addr"/>
      </template>
      <template #metric="{ s }">
        <oui-form-item-input :uci-section="s" name="metric" placeholder="0" :rules="{type: 'uinteger', min: 0, max: 255}"/>
      </template>
      <template #mtu="{ s }">
        <oui-form-item-input :uci-section="s" name="mtu" placeholder="1500" :rules="{type: 'uinteger', min: 64, max: 9000}"/>
      </template>
    </oui-typed-section>
    <oui-typed-section :title="$t('routes.Static IPv6 Routes')" type="route6" :columns="route6Columns" addremove>
      <template #interface="{ s }">
        <oui-form-item-select :uci-section="s" name="interface" :options="interfaces" required/>
      </template>
      <template #target="{ s }">
        <oui-form-item-input :uci-section="s" name="target" required rules="ip6addr"/>
      </template>
      <template #gateway="{ s }">
        <oui-form-item-input :uci-section="s" name="gateway" rules="ip6addr"/>
      </template>
      <template #metric="{ s }">
        <oui-form-item-input :uci-section="s" name="metric" placeholder="0" :rules="{type: 'uinteger', min: 0, max: 255}"/>
      </template>
      <template #mtu="{ s }">
        <oui-form-item-input :uci-section="s" name="mtu" placeholder="1500" :rules="{type: 'uinteger', min: 64, max: 9000}"/>
      </template>
    </oui-typed-section>
  </oui-form>
</template>

<script>
export default {
  data () {
    return {
      routeColumns: [
        { name: 'interface', label: this.$t('routes.Interface'), width: 160 },
        { name: 'target', label: this.$t('routes.Target') },
        { name: 'netmask', label: this.$t('IPv4-Netmask') },
        { name: 'gateway', label: this.$t('IPv4-Gateway') },
        { name: 'metric', label: this.$t('routes.Metric') },
        { name: 'mtu', label: 'MTU' }
      ],
      route6Columns: [
        { name: 'interface', label: this.$t('routes.Interface'), width: 160 },
        { name: 'target', label: this.$t('routes.Target') },
        { name: 'gateway', label: this.$t('IPv6-Gateway') },
        { name: 'metric', label: this.$t('routes.Metric') },
        { name: 'mtu', label: 'MTU' }
      ],
      interfaces: []
    }
  },
  created () {
    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces()
      this.interfaces = interfaces.map(item => item.name)
    })
  }
}
</script>
