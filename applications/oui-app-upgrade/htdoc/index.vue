<template>
  <n-upload ref="upload" directory-dnd :custom-request="customRequest">
    <n-upload-dragger>
      <div><n-icon size="48"><arrow-up-circle-icon/></n-icon></div>
      <n-text style="font-size: 16px">{{ $t('Click or drag files to this area to upload') }}</n-text>
    </n-upload-dragger>
  </n-upload>
  <n-modal v-model:show="modalConfirm" preset="dialog" :title="$t('Upgrade')"
    :positive-text="$t('OK')"
    :negative-text="$t('Cancel')"
    @positive-click="doUpgrade">
    <n-space vertical>
      <p>{{ $t('flash-confirm', { btn: this.$t('OK') }) }}</p>
      <n-text type="info">{{ this.$t('Size') + ': ' + bytesToHuman(this.size) }}</n-text>
      <n-text type="info">{{ 'MD5: ' + this.md5 }}</n-text>
      <n-checkbox v-model:checked="keepConfig">{{ $t('Keep settings and retain the current configuration') }}</n-checkbox>
    </n-space>
  </n-modal>
  <n-modal v-model:show="modalSpin" :close-on-esc="false" :mask-closable="false">
    <n-spin size="large">
      <template #description>
        <n-el style="color: var(--primary-color)">{{ $t('Upgrading') }}...</n-el>
      </template>
    </n-spin>
  </n-modal>
</template>

<script>
import { ArrowUpCircle as ArrowUpCircleIcon } from '@vicons/ionicons5'

export default {
  components: {
    ArrowUpCircleIcon
  },
  data() {
    return {
      modalConfirm: false,
      modalSpin: false,
      size: 0,
      md5: '',
      keepConfig: true
    }
  },
  methods: {
    bytesToHuman(bytes) {
      if (isNaN(bytes))
        return ''

      if (bytes < 0)
        return ''

      let units = ''

      const k = Math.floor((Math.log2(bytes) / 10))
      if (k > 0)
        units = 'KMGTPEZY'[k - 1] + 'iB'

      return (bytes / Math.pow(1024, k)).toFixed(2) + ' ' + units
    },
    customRequest(ctx) {
      const sid = this.$oui.state.sid
      this.axios.post(`/oui-upload?sid=${sid}&path=/tmp/firmware.bin`, ctx.file.file, {
        onUploadProgress: ev => ctx.onProgress({ percent: Math.ceil(ev.loaded * 100 / ev.total) })
      }).then(resp => {
        ctx.onFinish()
        this.size = resp.data.size
        this.md5 = resp.data.md5

        this.$refs.upload.clear()

        this.$oui.ubus('system', 'validate_firmware_image', { path: '/tmp/firmware.bin' }).then(({ valid }) => {
          if (!valid) {
            this.$dialog.error({
              content: this.$t('Firmware verification failed. Please upload the firmware again')
            })
          } else {
            this.keepConfig = true
            this.modalConfirm = true
          }
        })
      }).catch(error => ctx.onError(error))
    },
    doUpgrade() {
      this.$oui.call('system', 'sysupgrade', { keep: this.keepConfig }).then(() => {
        this.modalSpin = true
        this.$oui.reconnect().then(() => {
          this.$router.push('/login')
        })
      })
    }
  }
}
</script>

<i18n src="./locale.json"/>
