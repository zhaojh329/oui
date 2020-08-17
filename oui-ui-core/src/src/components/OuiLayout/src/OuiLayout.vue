<template>
  <a-layout>
    <a-layout-sider :style="{ overflow: 'auto', height: '100vh'}">
      <oui-side/>
    </a-layout-sider>
    <a-layout>
      <a-layout-content style="padding: 0 16px 16px; height: 100vh">
        <oui-header/>
        <div ref="oui-main-content" class="oui-main-content">
          <transition name="main" mode="out-in">
            <router-view></router-view>
          </transition>
          <a-back-top :target="() => $refs['oui-main-content']"/>
        </div>
        <div style="position: fixed; bottom: 10px; right: 40px">
          <a href="https://github.com/zhaojh329/oui" target="_blank">Powered by oui</a>
        </div>
      </a-layout-content>
    </a-layout>
  </a-layout>
</template>
<script>
import OuiSide from './OuiSide.vue'
import OuiHeader from './OuiHeader'

export default {
  components: {
    OuiSide,
    OuiHeader
  },
  computed: {
    hostname () {
      return this.$store.state.hostname
    }
  },
  watch: {
    hostname () {
      document.title = this.hostname + ' - oui'
    }
  },
  created () {
    this.$system.getBoardInfo().then(r => {
      this.$store.commit('setHostname', r.hostname)
    })
  }
}
</script>

<style scoped>
.main-enter, .main-leave-to {
  opacity: 0;
  transform: translateY(30px);
}

.main-enter-active {
  transition: all 0.2s;
}

.main-leave-active {
  position: absolute;
  transition: all 0.3s;
}

.oui-main-content {
  overflow: hidden;
  overflow-y: visible;
  padding: 5px;
  background-color: white;
  height: calc(100vh - 100px);
}
</style>
