<template>
  <Layout style="height: 100%">
    <Sider class="layout-side">
      <div class="logo">
        <router-link to="/">OpenWrt</router-link>
      </div>
      <Menu theme="dark" width="auto" accordion ref="menu" :open-names="openedNames" @on-open-change="onOpenChange">
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
  data () {
    return {
      menus: [],
      openedNames: [],
      breadcrumbs: [{title: 'Home'}]
    }
  },
  computed: {
    username() {
      return sessionStorage.getItem('username');
    }
  },
  methods: {
    handleUsrClick(name) {
      if (name === 'logout')
        this.$router.push('/login');
    },
    parseMenu(raw) {
      let menus = {};

      Object.keys(raw).map(key => {
        if (key.indexOf('/') < 0) {
          menus[key] = Object.assign({children: []}, raw[key]);
          delete(raw[key]);
        }
      });

      Object.keys(raw).map(key => {
        const paths = key.split('/');
        raw[key].path = '/' + key;

        menus[paths[0]].children.push(raw[key]);
      });

      menus = Object.keys(menus).map(k => Object.assign({path: '/' + k}, menus[k]));

      menus.sort((a, b) => a.index - b.index);

      menus.forEach(e => {
        e.children.sort((a, b) => a.index - b.index);
      });

      this.menus = menus;

      this.addRoutes();
    },
    addRoutes() {
      const routes = [];

      this.menus.forEach(e => {
        const route = {
          path: e.path,
          component: () => import('@/views/main.vue'),
          children: []
        };

        e.children.forEach(c => {
          route.children.push({
            path: c.path,
            component: () => import(`@/views/${c.view}.vue`),
            meta: {
              title: c.title,
              parentTitle: e.title
            }
          });
        });
        routes.push(route);
      });

      this.$router.addRoutes(routes);
    },
    onOpenChange(name) {
      this.openedNames[0] = name;
    }
  },
  watch: {
    '$route'(newRoute) {
      this.breadcrumbs = [{title: 'Home'}];

      if (newRoute.path === '/home') {
        this.openedNames = [];
        this.$nextTick(() => {
          this.$refs.menu.updateOpened()
        });

        return;
      }

      this.breadcrumbs[0].to = '/home';
      this.breadcrumbs.push({title: newRoute.meta.parentTitle});
      this.breadcrumbs.push({title: newRoute.meta.title});
    }
  },
  mounted() {
    this.$ubus.call('vwrt.ui', 'menu').then(r => {
      this.parseMenu(r.menu);
    });
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
