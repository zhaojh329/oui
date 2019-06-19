<template>
  <uci-form config="network">
    <uci-section :title="$t('Static IPv4 Routes')" type="route" addable table>
      <uci-option-list :label="$t('Interface')" name="interface" :options="interfaces" required></uci-option-list>
      <uci-option-input :label="$t('Target')" name="target" required rules="ip4addr"></uci-option-input>
      <uci-option-input :label="$t('IPv4-Netmask')" name="netmask" placeholder="255.255.255.255" required rules="netmask4"></uci-option-input>
      <uci-option-input :label="$t('IPv4-Gateway')" name="gateway" rules="ip4addr"></uci-option-input>
      <uci-option-input :label="$t('Metric')" name="metric" placeholder="0" :rules="{type: 'uinteger', min: 0, max: 255}"></uci-option-input>
      <uci-option-input label="MTU" name="mtu" placeholder="1500" :rules="{type: 'uinteger', min: 64, max: 9000}"></uci-option-input>
    </uci-section>
    <uci-section :title="$t('Static IPv6 Routes')" type="route6" addable table>
      <uci-option-list :label="$t('Interface')" name="interface" :options="interfaces" required></uci-option-list>
      <uci-option-input :label="$t('Target')" name="target" required rules="ip6addr"></uci-option-input>
      <uci-option-input :label="$t('IPv6-Gateway')" name="gateway" rules="ip6addr"></uci-option-input>
      <uci-option-input :label="$t('Metric')" name="metric" placeholder="0" :rules="{type: 'uinteger', min: 0, max: 255}"></uci-option-input>
      <uci-option-input label="MTU" name="mtu" placeholder="1500" :rules="{type: 'uinteger', min: 64, max: 9000}"></uci-option-input>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  data() {
    return {
      interfaces: []
    }
  },
  created() {
    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces();
      this.interfaces = interfaces.map(item => item.name);
    });
  }
}
</script>
