<template>
  <div>
    <div class="logo-name">
      <router-link to="/">{{ hostname }}</router-link>
    </div>
    <a-menu theme="dark" mode="inline" :open-keys="openKeys" @openChange="onOpenChange" @click="onClick" v-model="selectedKeys">
      <template v-for="menu in menus">
        <a-sub-menu v-if="menu.children" :key="menu.path">
          <template v-slot:title><span>{{ $t(menu.title) }}</span></template>
          <a-menu-item v-for="submenu in menu.children" :key="submenu.path">
            <span>{{ $t(submenu.title) }}</span>
          </a-menu-item>
        </a-sub-menu>
        <a-menu-item v-else :key="menu.path">
          <span>{{ $t(menu.title) }}</span>
        </a-menu-item>
      </template>
    </a-menu>
  </div>
</template>

<script>
import { mapState } from 'vuex'

export default {
  data () {
    return {
      openKeys: [],
      selectedKeys: []
    }
  },
  computed: {
    rootSubmenuKeys () {
      return this.menus.map(m => m.path)
    },
    ...mapState(['menus', 'hostname'])
  },
  watch: {
    '$route' () {
      this.updateSelection()
    }
  },
  methods: {
    updateSelection () {
      const path = this.$route.path
      if (path === '/home') {
        this.openKeys = []
        this.selectedKeys = []
        return
      }

      const paths = path.split('/')
      if (paths.length === 3) { this.openKeys = ['/' + paths[1]] } else { this.openKeys = [] }

      this.selectedKeys = [path]
    },
    onOpenChange (openKeys) {
      const latestOpenKey = openKeys.find(key => this.openKeys.indexOf(key) === -1)
      if (this.rootSubmenuKeys.indexOf(latestOpenKey) === -1) {
        this.openKeys = openKeys
      } else {
        this.openKeys = latestOpenKey ? [latestOpenKey] : []
      }
    },
    onClick (item) {
      if (item.key === this.$route.path) return
      this.$router.push(item.key)
    }
  },
  created () {
    this.$menu.load((menus, routes) => {
      this.$store.commit('setMenus', menus)
      this.$router.addRoutes(routes)
    })

    this.updateSelection()
  }
}
</script>

<style>
.logo-name {
  background-color: rgb(13, 41, 73) !important;
  line-height: 50px;
  text-align: center;
  font-size: 24px;
}
</style>
