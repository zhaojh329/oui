<template>
  <uci-form config="network">
    <uci-section title="Static IPv4 Routes" type="route" addable>
      <uci-option type="list" label="Interface" name="interface" :options="interfaces" required></uci-option>
      <uci-option type="input" label="Target" name="target" required></uci-option>
      <uci-option type="input" label="IPv4-Netmask" name="netmask" placeholder="255.255.255.255" required></uci-option>
      <uci-option type="input" label="IPv4-Gateway" name="gateway"></uci-option>
      <uci-option type="input" label="Metric" name="metric" placeholder="0"></uci-option>
      <uci-option type="input" label="MTU" name="mtu" placeholder="1500"></uci-option>
    </uci-section>
    <uci-section title="Static IPv6 Routes" type="route6" addable>
      <uci-option type="list" label="Interface" name="interface" :options="interfaces" required></uci-option>
      <uci-option type="input" label="Target" name="target" required></uci-option>
      <uci-option type="input" label="IPv6-Gateway" name="gateway"></uci-option>
      <uci-option type="input" label="Metric" name="metric" placeholder="0"></uci-option>
      <uci-option type="input" label="MTU" name="mtu" placeholder="1500"></uci-option>
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
