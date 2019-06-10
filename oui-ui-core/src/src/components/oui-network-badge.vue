<template>
  <div class="oui-network-badge">
    <div class="head">{{ iface }}</div>
    <div>{{ device }}</div>
  </div>
</template>

<script>
export default {
  name: 'OuiNetworkBadge',
  props: {
    iface: String,
    device: String
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
    background-color: rgb(204, 204, 204);
    padding: 0 3px;
  }
}
</style>
