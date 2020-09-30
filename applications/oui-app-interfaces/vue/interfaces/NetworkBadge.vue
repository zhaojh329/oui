<template>
  <div class="oui-network-badge">
    <div class="head" :style="{backgroundColor: color}">{{ iface }}</div>
    <div>{{ device }}</div>
  </div>
</template>

<script>
export default {
  name: 'NetworkBadge',
  props: {
    iface: String,
    device: String
  },
  data () {
    return {
      color: '#eeeeee'
    }
  },
  created () {
    this.$firewall.load().then(() => {
      const z = this.$firewall.findZoneByNetwork(this.iface)
      if (z) { this.color = z.color() }
    })
  }
}
</script>

<style>
  .oui-network-badge {
    border: 1px solid #ddd;
    border-radius: 3px;
    background-color: #fff;
    text-align: center;
    white-space: nowrap;
  }
  .oui-network-badge .head {
    border-bottom: 1px solid #ddd;
    padding: 0 3px;
  }
</style>
