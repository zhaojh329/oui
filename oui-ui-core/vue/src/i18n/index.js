import Vue from 'vue'
import VueI18n from 'vue-i18n'

Vue.use(VueI18n)

const i18n = new VueI18n({
  locale: 'en',
  fallbackLocale: 'en',
  messages: {}
})

i18n.loaded = function (locale) {
  return !!this.messages[locale || this.locale]
}

export default i18n
