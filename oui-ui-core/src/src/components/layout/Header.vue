<template>
  <div>
    <el-header class="header" height="40px">
      <el-breadcrumb separator="/" class="crumbs">
        <el-breadcrumb-item v-for="(item, i) in breadcrumbs" :key="i" :to="item.to">{{ item.title }}</el-breadcrumb-item>
      </el-breadcrumb>
      <div class="header-right">
        <el-tooltip effect="dark" :content="isfullScreen ? 'Exit Full Screen' : 'Full Screen'" placement="bottom">
          <i class="iconfont iconfull" @click="fullScreen"></i>
        </el-tooltip>
        <el-dropdown @command="onUserCommand">
          <span class="user">
            {{ username }}<i class="el-icon-arrow-down el-icon--right"></i>
          </span>
          <el-dropdown-menu slot="dropdown">
            <el-dropdown-item command="logout">Logout</el-dropdown-item>
            <el-dropdown-item command="reboot">Reboot</el-dropdown-item>
          </el-dropdown-menu>
        </el-dropdown>
      </div>
    </el-header>
  </div>
</template>

<script>
export default {
  name: 'Header',
  data() {
    return {
      username: '',
      isfullScreen: false,
      breadcrumbs: []
    }
  },
  methods: {
    fullScreen() {
      if (this.isfullScreen) {
        if (document.exitFullscreen) {
          document.exitFullscreen()
        } else if (document.mozCancelFullScreen) {
          document.mozCancelFullScreen()
        } else if (document.webkitCancelFullScreen) {
          document.webkitCancelFullScreen()
        } else if (document.msExitFullscreen) {
          document.msExitFullscreen()
        }
      } else {
        const docElm = document.documentElement
        if (docElm.requestFullscreen) {
          docElm.requestFullscreen()
        } else if (docElm.mozRequestFullScreen) {
          docElm.mozRequestFullScreen()
        } else if (docElm.webkitRequestFullScreen) {
          docElm.webkitRequestFullScreen()
        } else if (docElm.msRequestFullscreen) {
          docElm.msRequestFullscreen()
        }
      }

      this.isfullScreen = !this.isfullScreen;
    },
    getBreadCrumbList(route) {
      const homeRoute = this.$router.options.routes[1].children[0];
      const homeItem = {title: homeRoute.meta.title};
      const matched = route.matched;

      if (matched.some(item => item.path === '/home')) {
        return [homeItem];
      }

      homeItem.to = '/home';

      return [homeItem, matched[0].meta, matched[1].meta];
    },
    onUserCommand(cmd) {
      if (cmd === 'logout') {
        this.$router.push('/login');
      } else if (cmd === 'reboot') {
        this.$system.reboot().then(() => {
          this.$reconnect('Rebooting...');
        });
      }
    }
  },
  watch: {
    '$route'(newRoute) {
      this.breadcrumbs = this.getBreadCrumbList(newRoute);
    }
  },
  created() {
    this.breadcrumbs = this.getBreadCrumbList(this.$route);

    this.$session.get(r => {
      this.username = r.username;
    });
  }
}
</script>

<style lang="scss">
.header {
  display: flex;
  justify-content: space-between;
  align-items: center;

  .el-breadcrumb {
    font-size: 24px;
  }

  .header-right {
    display: flex;

    .iconfull {
      font-size: 24px;
      cursor: pointer;
      margin-top: 4px;
    }

    .user {
      cursor: pointer;
      font-size: 24px;
    }

    > * {
      margin-right: 20px;
    }
  }
}
</style>
