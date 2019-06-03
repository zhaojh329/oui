<template>
  <el-container style="height: 100%;">
    <Aside></Aside>
    <el-container>
      <Header></Header>
      <el-main>
        <transition name="main" mode="out-in">
          <router-view></router-view>
        </transition>
      </el-main>
      <el-footer class="footer" height="40px">
        <a href="https://github.com/zhaojh329/oui" target="_blank">Powered by oui</a>
      </el-footer>
    </el-container>
  </el-container>
</template>

<script>
import Aside from './Aside.vue'
import Header from './Header.vue'

export default {
  name: 'Layout',
  components: {
    Aside,
    Header
  },
  created() {
    this.$system.getBoardInfo().then(r => {
      document.title = r.hostname + ' - oui';
    });
  }
}
</script>

<style lang="scss">
.el-main {
  background-color: white;
}

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

.footer {
  line-height: 40px;
  font-size: 14px;
  color: #999999;
  text-align: right;
  padding-right: 100px;
}
</style>
