import md5 from 'js-md5'

export default {
  install (Vue) {
    Vue.prototype.$reconnect = function (title) {
      this.$spin(title)

      let interval

      const img = document.createElement('img')
      img.addEventListener('load', () => {
        window.clearInterval(interval)
        this.$spin(false)
        this.$router.push('/login')
      })

      window.setTimeout(() => {
        interval = window.setInterval(() => {
          img.src = '/favicon.ico?r=' + Math.random()
        }, 1000)
      }, 5000)
    }

    Vue.prototype.$spin = function (config) {
      this.$store.commit('spin', config)
    }

    Vue.prototype.$md5 = md5
  }
}
