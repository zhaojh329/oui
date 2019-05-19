<template>
  <Layout style="height: 100%">
    <Sider class="layout-side">
      <div class="logo">
        <router-link to="/">OpenWrt</router-link>
      </div>
      <Menu theme="dark" width="auto" accordion>
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
        <Dropdown trigger="click" @on-click="handleUsrClick" style="float: right">
          <Avatar style="background-color: #1890ff" icon="ios-person" />
          <Icon :size="18" type="md-arrow-dropdown"></Icon>
          <DropdownMenu slot="list">
            <DropdownItem class="user">{{ username }}</DropdownItem>
            <DropdownItem>Logout</DropdownItem>
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
      menus: []
    }
  },
  computed: {
    username() {
      return sessionStorage.getItem('username');
    }
  },
  methods: {
    handleUsrClick() {
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
            component: () => import(`@/views/${c.view}.vue`)
          });
        });
        routes.push(route);
      });

      this.$router.addRoutes(routes);
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
