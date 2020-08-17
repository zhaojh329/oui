<template>
  <a-config-provider id="app" :locale="locale">
    <fullscreen ref="fullscreen">
      <a-spin :spinning="spinning > 0" :tip="spintip || $t('Loading...')" size="large">
        <router-view></router-view>
      </a-spin>
    </fullscreen>
  </a-config-provider>
</template>

<script>
import zhCN from 'ant-design-vue/es/locale-provider/zh_CN'
import { mapState } from 'vuex'

export default {
  computed: {
    locale () {
      if (this.$i18n.locale === 'zh-CN') { return zhCN } else return undefined
    },
    ...mapState(['fullscreen', 'spinning', 'spintip'])
  },
  watch: {
    fullscreen () {
      this.$refs.fullscreen.toggle()
    }
  }
}
</script>
