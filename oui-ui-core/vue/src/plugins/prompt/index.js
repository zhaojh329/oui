import Prompt from './Prompt.vue'

export default {
  install (Vue) {
    Vue.prototype.$prompt = function (config) {
      return new Promise((resolve, reject) => {
        let value = ''
        let valid = true

        const confirm = this.$confirm({
          autoFocusButton: null,
          title: config.title,
          content: h => h(Prompt, {
            props: {
              placeholder: config.placeholder,
              validator: config.validator
            },
            on: {
              enter: () => {
                if (!valid || !value) return
                confirm.destroy()
                resolve(value)
              },
              validator: v => {
                valid = v
              },
              change: v => {
                value = v
              }
            }
          }),
          onOk: close => {
            if (!valid || !value) return
            close()
            resolve(value)
          },
          onCancel: () => reject(new Error())
        })
      })
    }
  }
}
