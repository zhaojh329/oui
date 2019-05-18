<template>
  <Layout style="height: 100%">
    <Sider class="layout-side">
      <div class="logo">
        <router-link to="/">OpenWrt</router-link>
      </div>
      <Menu theme="dark" width="auto" accordion>
        <Submenu v-for="item in menuList" :name="item.name" :key="item.name">
            <template slot="title">
              <Icon v-if="item.icon" :type="item.icon"></Icon>{{ item.title }}
            </template>
            <MenuItem v-for="subItem in item.children" :name="item.name + subItem.name" :key="item.name + subItem.name" :to="subItem.path">{{subItem.title}}</MenuItem>
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
    }
  },
  computed: {
    menuList() {
      return this.$store.state.menus;
    },
    username() {
      return sessionStorage.getItem('username');
    }
  },
  methods: {
    handleUsrClick() {
      this.$router.push('/login');
    }
  },
  mounted() {
    console.log('main mounted');
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