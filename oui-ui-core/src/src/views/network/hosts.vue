<template>
  <uci-form config="dhcp" @apply="onApply">
    <uci-section title="Hostnames" type="domain" addable table>
      <uci-option type="input" label="Hostname" name="name" required></uci-option>
      <uci-option type="input" label="IP address" name="ip" required></uci-option>
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
