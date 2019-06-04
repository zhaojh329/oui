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
      <el-backtop target=".el-main" class="backtop">
        <div class="text">UP</div>
        <i class="el-icon-caret-top"></i>
      </el-backtop>
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

.backtop {
  display: flex;
  flex-direction: column;

  .text {
    height: 100%;
    width: 100%;
    background-color: #f2f5f6;
    box-shadow: 0 0 6px rgba(0,0,0, .12);
    text-align: center;
    line-height: 40px;
    color: #1989fa;
  }
}
</style>
