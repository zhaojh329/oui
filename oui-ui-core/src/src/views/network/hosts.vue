<template>
  <oui-form uci-config="dhcp" @applied="onApplied">
    <oui-typed-section type="domain" addremove :columns="columns">
      <template #name="{ s }">
        <oui-form-item-input :uci-section="s" name="name" required rules="hostname"/>
      </template>
      <template #ip="{ s }">
        <oui-form-item-input :uci-section="s" name="ip" required rules="ipaddr"/>
      </template>
    </oui-typed-section>
  </oui-form>
</template>

<script>
export default {
  data () {
    return {
      columns: [
        { name: 'name', label: this.$t('Hostname') },
        { name: 'ip', label: this.$t('IP address') }
      ]
    }
  },
  methods: {
    onApplied () {
      this.$system.initRestart('dnsmasq')
    }
  }
}
</script>
