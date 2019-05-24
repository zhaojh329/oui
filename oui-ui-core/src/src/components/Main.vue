<template>
  <Layout style="height: 100%">
    <Sider class="layout-side">
      <div class="logo">
        <router-link to="/">OpenWrt</router-link>
      </div>
      <Menu theme="dark" width="auto" accordion ref="menu" :open-names="openedNames" :active-name="activeName" @on-select="onMenuSelect" @on-open-change="onMenuOpenChange">
        <Submenu v-for="item in menus" :name="item.path" :key="item.path">
            <template slot="title">
              <Icon v-if="item.icon" :type="item.icon"></Icon>{{ item.title }}
            </template>
            <MenuItem v-for="subItem in item.children" :name="item.path + subItem.path" :key="item.path + subItem.path" :to="subItem.path">{{subItem.title}}</MenuItem>
        </Submenu>
      </Menu>
    </Sider>
    <Layout>
      <Header class="layout-header-bar">
        <Breadcrumb>
          <BreadcrumbItem v-for="(item, i) in breadcrumbs" :key="i" :to="item.to">{{ item.title }}</BreadcrumbItem>
        </Breadcrumb>
        <Dropdown trigger="click" @on-click="handleUsrClick" style="float: right">
          <Avatar style="background-color: #1890ff" icon="ios-person" />
          <Icon :size="18" type="md-arrow-dropdown"></Icon>
          <DropdownMenu slot="list">
            <DropdownItem class="user">{{ username }}</DropdownItem>
            <DropdownItem name="logout">Logout</DropdownItem>
          </DropdownMenu>
        </Dropdown>
        <Dropdown trigger="click" style="float: right">
          <a href="javascript:void(0)">Lang<Icon type="ios-arrow-down"></Icon></a>
          <DropdownMenu slot="list">
            <DropdownItem>简体中文</DropdownItem>
            <DropdownItem>English</DropdownItem>
          </DropdownMenu>
        </Dropdown>
      </Header>
      <Content class="layout-content">
        <router-view />
      </Content>
    </Layout>
  </Layout>
</template>

<script>
export default {
  data() {
    return {
      username: '',
      openedNames: [],
      activeName: '',
      breadcrumbs: [{title: 'Home'}]
    }
  },
  computed: {
    menus() {
      return this.$store.state.menus;
    }
  },
  methods: {
    handleUsrClick(name) {
      if (name === 'logout')
        this.$router.push('/login');
    },
    onMenuSelect(name) {
      this.activeName = name;
    },
    onMenuOpenChange(names) {
      this.openedNames = names;
    }
  },
  watch: {
    '$route'(newRoute) {
      this.breadcrumbs = [{title: 'Home'}];

      if (newRoute.path === '/home') {
        this.openedNames = [];
        this.activeName = '';
        this.$nextTick(() => {
          this.$refs.menu.updateOpened();
          this.$refs.menu.updateActiveName();
        });

        return;
      }

      this.breadcrumbs[0].to = '/home';
      this.breadcrumbs.push({title: newRoute.meta.parentTitle});
      this.breadcrumbs.push({title: newRoute.meta.title});
    }
  },
  created() {
    this.$ubus.call('system', 'board').then(r => {
      document.title = r.hostname + ' - oui';
    });

    this.$session.get(r => {
      this.username = r.username;
    });

    this.$menu.load((menus, routes) => {
      this.$store.commit('setMenus', menus);
      this.$router.addRoutes(routes);
    });
  },
  mounted() {
    const route = this.$route;
    const paths = route.path.split('/');
    if (paths.length === 3) {
      this.openedNames = ['/' + paths[1]];
      this.activeName = this.openedNames[0] + route.path;

      this.$nextTick(() => {
        this.$refs.menu.updateOpened();
        this.$refs.menu.updateActiveName();
      });

      this.breadcrumbs = [{title: 'Home'}];
      this.breadcrumbs[0].to = '/home';
      this.breadcrumbs.push({title: route.meta.parentTitle});
      this.breadcrumbs.push({title: route.meta.title});
    }
  }
}
</script>

<style lang="less" scoped>
.layout-side {
  height: 100vh;
  overflow-y: auto;
}

.logo {
  background: #5b6270;
  border-radius: 3px;
  text-align: center;
  font-size: large;
  height: 40px;
  width: 150px;
  left: 20px;
  position: relative;
  top: 10px;
}

.layout-header-bar {
  padding: 0;
  background: #fff;
  padding-right: 30px;
  & > * {
    display: inline-block;
    margin: 0 10px 0 10px;
  }
}

.user:hover {
  background: white;
}

.layout-content {
  margin: 18px;
  overflow: auto;
}
</style>
