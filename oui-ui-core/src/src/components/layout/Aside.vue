<template>
  <div>
    <el-aside class="asside-nav">
      <div class="logo-name">
        <router-link to="/">OpenWrt</router-link>
      </div>
      <el-menu :default-active="$route.path" background-color="#03152A" text-color="rgba(255,255,255,.8)" active-text-color="#ffffff" router unique-opened>
        <el-submenu v-for="item in menus" :key="item.path" :index="item.path">
          <template slot="title">
            <span slot="title">{{ item.title }}</span>
          </template>
          <el-menu-item v-for="subItem in item.children" :key="item.path + subItem.path" :index="subItem.path">{{ subItem.title }}</el-menu-item>
        </el-submenu>
      </el-menu>
    </el-aside>
  </div>
</template>

<script>
export default {
  name: 'Aside',
  computed: {
    menus() {
      return this.$store.state.menus;
    }
  },
  created() {
    this.$menu.load((menus, routes) => {
      this.$store.commit('setMenus', menus);
      this.$router.addRoutes(routes);
    });
  }
}
</script>

<style lang="scss">
.asside-nav {
  width: auto !important;
  height: 100%;
  display: flex;
  flex-direction: column;

  .logo-name {
    background-color: rgb(13, 41, 73) !important;
    line-height: 50px;
    text-align: center;
    font-size: 24px;
  }

  .el-menu {
    width: 200px;
    flex: 1;
    overflow: inherit;
    border-right: none;
    &::-webkit-scrollbar {
      display: none;
    }

    .el-menu-item {
      &:hover {
        color: #ffffff !important;
        background-color: #375573 !important;
      }
    }

    .el-menu-item.is-active {
      background-color: #56a9ff !important
    }
  }
}
</style>
