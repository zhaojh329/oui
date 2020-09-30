<template>
  <oui-form uci-config="oui-bwm">
    <oui-typed-section :title="$t('Bandwidth Monitor')" type="bwm" :collapsible="false" v-slot="{ s }">
      <oui-form-item-input :uci-section="s" label="TTL" name="ttl" placeholder="30" rules="uinteger" append="s"/>
      <oui-form-item-list :uci-section="s" :label="$t('oui-bwm.Local network')" name="local_network" :help="$t('oui-bwm.local_network_help')"/>
    </oui-typed-section>
  </oui-form>
</template>
