<template>
  <oui-form uci-config="oui-bwm">
    <oui-typed-section :title="$t('Bandwidth Monitor')" type="bwm" :collapsible="false" v-slot="{ s }">
      <oui-form-item-input :uci-section="s" label="TTL" name="ttl" placeholder="30" rules="uinteger" append="s"/>
      <oui-form-item-list :uci-section="s" :label="$t('Local network')" name="local_network" :help="$t('192.168.1.1/24 or a logical interface name')"/>
    </oui-typed-section>
  </oui-form>
</template>

<script>
export default {
}
</script>
