<template>
  <div>
    <h1>{{ content }}</h1>
  </div>
</template>

<script>

export default {
  name: 'error_404',
  data() {
    return {
      content: ''
    }
  },
  computed: {
    menus() {
      return this.$store.state.menus;
    }
  },
  beforeRouteEnter(to, from, next) {
    next(vm => {
      if (vm.menus.length === 0) {
        vm.$menu.load((menus, routes) => {
          vm.$store.commit('setMenus', menus);
          vm.$router.addRoutes(routes);
          vm.$router.push(to.path);
        });
      } else {
        vm.content = '404 Not Fount';
      }
    });
  }
}
</script>
