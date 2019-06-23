export default {
  install(Vue) {
    Vue.prototype.$bus = new Vue();

    Vue.prototype.$getParent = function(name, depth) {
      let parent = this.$parent;

      if (typeof(name) === 'undefined')
        return parent;

      if (typeof(depth) === 'undefined')
        depth = 20;

      while (typeof(parent) !== 'undefined' && depth-- > 0) {
        if (parent.$options.name === name)
          return parent;

        if (depth === 0)
          return null;

        parent = parent.$parent;
      }

      return parent;
    }

    Vue.prototype.$reconnect = function(title) {
      const loading = this.$getLoading(title);

      const vm = this;

      function check() {
        vm.axios.get('/favicon.ico').then(() => {
          loading.close();
          vm.$router.push('/login');
        }).catch(() => {
          window.setTimeout(check, 5000);
        });
      }

      window.setTimeout(check, 5000);
    }

    Vue.prototype.$getLang = function() {
      this.$ubus.call('oui.ui', 'lang').then(r => {
        let lang = r.lang;

        this.$store.commit('setLang', lang);

        if (lang === 'auto')
          lang = navigator.language;
        this.$i18n.locale = lang;
      });
    }

    Vue.prototype.$getLoading = function(text) {
      return this.$loading({
        text: text || this.$t('Loading...'),
        spinner: 'el-icon-loading',
        background: 'rgba(0, 0, 0, 0.7)'
      });
    }
  }
}
