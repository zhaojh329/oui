<template>
  <el-container class="oui-container" style="height: calc(100vh - 16px);">
    <el-aside width="auto">
      <el-scrollbar>
        <div v-if="!isCollapse" style="text-align: center;">
          <el-link type="primary" @click="$router.push('/')" :underline="false" style="font-size: large;">{{ $oui.state.hostname }}</el-link>
        </div>
        <el-divider v-if="!isCollapse"/>
        <el-menu unique-opened router :default-active="selectedMenu" :collapse="isCollapse">
          <template v-for="menu in menus" :key="menu.path">
            <el-sub-menu v-if="menu.children" :index="menu.path">
              <template #title>
                <el-icon v-if="menu.svg"><div v-html="renderSvg('svg', menu.svg)"></div></el-icon>
                <span>{{ $t('menus.' + menu.title) }}</span>
              </template>
              <el-menu-item v-for="submenu in menu.children" :key="submenu.path" :index="submenu.path">
                <template #title>{{ $t('menus.' + submenu.title) }}</template>
              </el-menu-item>
            </el-sub-menu>
            <el-menu-item v-else :index="menu.path">
              <el-icon v-if="menu.svg"><div v-html="renderSvg('svg', menu.svg)"></div></el-icon>
              <template #title>{{ $t('menus.' + menu.title) }}</template>
            </el-menu-item>
          </template>
        </el-menu>
      </el-scrollbar>
    </el-aside>
    <el-container>
      <el-header>
        <el-icon @click="isCollapse = !isCollapse" class="collapse-icon" :size="25">
          <component :is="isCollapse ? 'Expand' : 'Fold'"/>
        </el-icon>
        <el-space size="large">
          <el-icon color="#ffd93b" size="24" style="cursor: pointer;" @click="$oui.state.isDark = !$oui.state.isDark">
            <component :is="$oui.state.isDark ? MoonIcon : SunnySharpIcon"/>
          </el-icon>
          <el-dropdown @command="lang => $oui.setLocale(lang)">
            <span class="el-dropdown-link"><el-icon><TranslateIcon/></el-icon></span>
            <template #dropdown>
              <el-dropdown-menu>
                <el-dropdown-item v-for="i in localeOptions" :key="i.key" :command="i.key" :class="{selected: i.key === $oui.state.locale}">{{ i.label }}</el-dropdown-item>
              </el-dropdown-menu>
            </template>
          </el-dropdown>
          <el-dropdown @command="handleUserAction">
            <span class="el-dropdown-link"><el-icon><Avatar/></el-icon></span>
            <template #dropdown>
              <el-dropdown-menu>
                <el-dropdown-item command="logout" :icon="LogoutIcon">{{ $t('Logout') }}</el-dropdown-item>
                <el-dropdown-item command="reboot" icon="SwitchButton">{{ $t('Reboot') }}</el-dropdown-item>
              </el-dropdown-menu>
            </template>
          </el-dropdown>
        </el-space>
      </el-header>
      <el-main>
        <el-scrollbar>
          <router-view/>
          <el-backtop target=".oui-container .el-container .el-scrollbar__wrap"/>
        </el-scrollbar>
      </el-main>
      <el-footer>
        <div class="copyright">
          <p>Copyright © 2022 Powered by <a href="https://github.com/zhaojh329/oui" target="_blank">oui</a></p>
        </div>
      </el-footer>
    </el-container>
  </el-container>
</template>

<script>
import {
  Translate as TranslateIcon
} from '@vicons/carbon'

import {
  Moon as MoonIcon,
  SunnySharp as SunnySharpIcon,
  LogOutOutline as LogoutIcon
} from '@vicons/ionicons5'

export default {
  props: {
    menus: {
      type: Array,
      required: true
    }
  },
  components: {
    TranslateIcon
  },
  data() {
    return {
      selectedMenu: '',
      isCollapse: false
    }
  },
  setup() {
    return {
      LogoutIcon,
      MoonIcon,
      SunnySharpIcon
    }
  },
  computed: {
    localeOptions() {
      const titles = {
        'en-US': 'English',
        'ja-JP': '日本語',
        'zh-CN': '简体中文',
        'zh-TW': '繁體中文'
      }

      const options = this.$i18n.availableLocales.map(locale => {
        return {
          label: titles[locale] ?? locale,
          key: locale
        }
      })

      options.unshift({
        label: this.$t('Auto'),
        key: 'auto'
      })

      return options
    }
  },
  methods: {
    renderSvg(el, opt) {
      const props = []
      const children = []

      Object.keys(opt).forEach(key => {
        if (key.startsWith('-')) {
          props.push(`${key.substring(1)}="${opt[key]}"`)
        } else {
          if (Array.isArray(opt[key]))
            opt[key].forEach(item => children.push(this.renderSvg(key, item)))
          else
            children.push(this.renderSvg(key, opt[key]))
        }
      })

      return `<${el} ${props.join(' ')}>${children.join(' ')}</${el}>`
    },
    handleUserAction(command) {
      if (command === 'logout') {
        this.$oui.logout()
        this.$router.push('/login')
      } else if (command === 'reboot') {
        this.$confirm(this.$t('RebootConfirm'), this.$t('Reboot'), {
          type: 'warning'
        }).then(() => {
          this.$oui.ubus('system', 'reboot').then(() => {
            const loading = this.$loading({
              lock: true,
              text: this.$t('Rebooting') + '...',
              background: 'rgba(0, 0, 0, 0.7)'
            })

            this.$oui.reconnect().then(() => {
              loading.close()
              this.$router.push('/login')
            })
          })
        })
      }
    }
  },
  mounted() {
    this.selectedMenu = this.$route.path
  }
}
</script>

<style scoped>
.oui-container .el-menu {
  border-right: none;
}

.oui-container .el-aside {
  border-right: 1px var(--el-border-color) var(--el-border-style);
}

.oui-container .el-header {
  height: 35px;
}

.oui-container .el-main {
  border: 1px var(--el-border-color) var(--el-border-style);
}

:deep(.el-dropdown-menu__item).selected {
  background-color: var(--el-dropdown-menuItem-hover-fill);
  color: var(--el-dropdown-menuItem-hover-color);
}

.el-header {
  display: flex;
  justify-content: space-between;
}

.collapse-icon {
  color: var(--el-color-primary);
  cursor: pointer;
}

.copyright {
  text-align: right;
  font-size: 1.2em;
  color: #888;
}

.el-dropdown-link {
  color: var(--el-color-primary);
  cursor: pointer;
}

.el-dropdown-link:focus {
  outline: none;
}

.el-dropdown-link .el-icon {
  font-size: 24px;
}
</style>

<i18n src="./locale.json"/>
