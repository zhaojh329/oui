import RebootDialog from './RebootDialog.vue'

export default {
  install(Vue) {
    Vue.prototype.$reboot = function() {
      let dialog = new Vue({
        data: {
          show: true
        },
        render(h) {
          return h(RebootDialog, {
            on: {
              close: () => {
                this.destroy();
              }
            }});
        },
        methods: {
          destroy() {
            setTimeout(() => {
              this.$destroy();
              document.body.removeChild(this.$el);
            }, 300);
          }
        }
      }).$mount();
      document.body.appendChild(dialog.$el);
    }
  }
};
