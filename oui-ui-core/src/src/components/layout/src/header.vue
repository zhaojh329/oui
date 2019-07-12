<template>
  <div>
    <el-header class="header" height="40px">
      <el-breadcrumb separator="/" class="crumbs">
        <el-breadcrumb-item v-for="(item, i) in breadcrumbs" :key="i" :to="item.to">{{ $t(item.title) }}</el-breadcrumb-item>
      </el-breadcrumb>
      <div class="header-right">
        <el-tooltip effect="dark" :content="isfullScreen ? $t('Exit Full Screen') : $t('Full Screen')" placement="bottom">
          <i class="iconfont iconfull" @click="fullScreen"></i>
        </el-tooltip>
        <el-dropdown @command="onLangCommand">
          <span class="arrow-down">
            {{ $t('Language') }}<i class="el-icon-arrow-down"></i>
          </span>
          <el-dropdown-menu slot="dropdown">
            <el-dropdown-item command="en" :icon="(lang === 'en') ? 'el-icon-arrow-right' : ''">English</el-dropdown-item>
            <el-dropdown-item command="zh-CN" :icon="(lang === 'zh-CN') ? 'el-icon-arrow-right' : ''">中文</el-dropdown-item>
            <el-dropdown-item command="auto" :icon="(lang === 'auto') ? 'el-icon-arrow-right' : ''">{{ $t('Automatic') }}</el-dropdown-item>
          </el-dropdown-menu>
        </el-dropdown>
        <el-dropdown @command="onUserCommand">
          <span class="arrow-down">
            {{ username }}<i class="el-icon-arrow-down"></i>
          </span>
          <el-dropdown-menu slot="dropdown">
            <el-dropdown-item command="logout">{{ $t('Logout') }}</el-dropdown-item>
            <el-dropdown-item command="reboot">{{ $t('Reboot') }}</el-dropdown-item>
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
  computed: {
    lang() {
      return this.$store.state.lang;
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

      if (matched.some(item => item.path === '/home'))
        return [homeItem];

      homeItem.to = '/home';

      const list = [homeItem, matched[0].meta];

      if (!matched[0].redirect)
        list.push(matched[1].meta);

      return list;
    },
    onLangCommand(cmd) {
      this.$ubus.call('oui.ui', 'lang', {lang: cmd}).then(({lang}) => {
        this.$store.commit('setLang', lang);
        if (lang === 'auto')
          lang = navigator.language;
        this.$i18n.locale = lang;
      });
    },
    onUserCommand(cmd) {
      if (cmd === 'logout') {
        this.$router.push('/login');
      } else if (cmd === 'reboot') {
        this.$confirm(this.$t('Really reboot now?'), this.$t('Reboot')).then(() => {
          this.$system.reboot().then(() => {
            this.$reconnect('Rebooting...');
          });
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

    .arrow-down {
      cursor: pointer;
      font-size: 24px;
    }

    > * {
      margin-right: 20px;
    }
  }
}
</style>
