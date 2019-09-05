<template>
  <uci-form config="dhcp" @applied="onApplied">
    <uci-section type="domain" addable table>
      <uci-option-input :label="$t('Hostname')" name="name" required rules="hostname"></uci-option-input>
      <uci-option-input :label="$t('IP address')" name="ip" required rules="ipaddr"></uci-option-input>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  methods: {
    onApplied() {
      this.$system.initRestart('dnsmasq');
    }
  }
}
</script>
