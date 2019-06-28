<template>
  <div class="oui-network-badge">
    <div class="head" :style="{backgroundColor: color}">{{ iface }}</div>
    <div>{{ device }}</div>
  </div>
</template>

<script>
export default {
  name: 'OuiNetworkBadge',
  props: {
    iface: String,
    device: String
  },
  data() {
    return {
      color: '#eeeeee'
    }
  },
  created() {
    this.$firewall.load().then(() => {
      const z = this.$firewall.findZoneByNetwork(this.iface);
      if (z)
        this.color = z.color();
    });
  }
}
</script>

<style lang="scss">
.oui-network-badge {
  border: 1px solid #DDDDDD;
  border-radius: 3px;
  background-color: white;
  text-align: center;
  white-space: nowrap;

  .head {
    border-bottom: 1px solid #DDDDDD;
    padding: 0 3px;
  }
}
</style>
