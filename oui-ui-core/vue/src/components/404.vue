<template>
  <div v-if="loaded">
    <div style="font-size: 200px">404</div>
    <div style="font-size: 40px">{{ $t('Sorry, the page you visited does not exist!') }}</div>
  </div>
</template>

<script>
export default {
  data () {
    return {
      loaded: false
    }
  },
  computed: {
    menus () {
      return this.$store.state.menus
    }
  },
  beforeRouteEnter (to, from, next) {
    next(vm => {
      if (vm.menus.length === 0) {
        vm.$menu.load((menus, routes) => {
          vm.$store.commit('setMenus', menus)
          vm.$router.addRoutes(routes)
          vm.$router.push(to.path)
        })
      } else {
        vm.loaded = true
      }
    })
  }
}
</script>
