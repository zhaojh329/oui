<template>
  <uci-form config="dhcp" @apply="onApply">
    <uci-section :title="$t('Hostnames')" type="domain" addable table>
      <uci-option type="input" :label="$t('Hostname')" name="name" required rules="hostname"></uci-option>
      <uci-option type="input" :label="$t('IP address')" name="ip" required rules="ipaddr"></uci-option>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  methods: {
    onApply() {
      this.$system.initRestart('dnsmasq');
    }
  }
}
</script>
