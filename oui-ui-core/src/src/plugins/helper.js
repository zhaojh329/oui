export default {
  install (Vue) {
    Vue.prototype.$reconnect = function (title) {
      const vm = this

      this.$spin(title)

      function check () {
        vm.$ubus.call('oui.ui', 'lang', {}, 1).then(() => {
          this.$spin(false)
          vm.$router.push('/login')
        }).catch(() => {
          window.setTimeout(check, 1000)
        })
      }

      window.setTimeout(check, 5000)
    }

    Vue.prototype.$spin = function (config) {
      this.$store.commit('spin', config)
    }
  }
}
