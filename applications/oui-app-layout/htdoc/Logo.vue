<template>
  <router-link to="/" style="text-decoration: none;">
    <div class="logo">
        <img src="./openwrt.png" alt="" :class="{'mr-2': !collapsed}" />
        <h2 v-show="!collapsed" class="title">{{ $oui.state.hostname }}</h2>
    </div>
  </router-link>
</template>

<script lang="ts">
export default {
  name: 'Index',
  props: {
    collapsed: {
      type: Boolean
    }
  },
  data() {
    return {}
  }
}
</script>

<style scoped>
.logo {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 64px;
  line-height: 64px;
  overflow: hidden;
  white-space: nowrap;
}
.logo img {
  width: auto;
  height: 32px;
}
.logo .title {
  margin: 0;
  color: black;
}
</style>
