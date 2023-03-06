<template>
  <router-link to="/" style="text-decoration: none;" :class="{ dark: $oui.state.theme === 'dark' }">
    <div class="logo">
      <img :class="{'mr-2': !collapsed}"/>
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
  content: url(./openwrt-logo-black.svg);
}
.dark .logo img {
  width: auto;
  height: 32px;
  content: url(./openwrt-logo-white.svg);
}
.logo .title {
  margin: 0;
  color: black;
}
.dark .logo .title {
  margin: 0;
  color: white;
}
</style>
