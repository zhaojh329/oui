<template>
  <el-upload v-loading="loading" drag action="/oui-upload" :data="{sid: $oui.state.sid, path: '/tmp/firmware.bin'}"
    :show-file-list="false" :on-success="onUploadSuccess" :before-upload="() => loading = true">
    <el-icon size="30"><upload-filled/></el-icon>
    <div>{{ $t('Click or drag files to this area to upload') }}</div>
  </el-upload>
  <el-dialog v-model="modalConfirm" :title="$t('Upgrade')">
    <el-space direction="vertical" fill>
      <p>{{ $t('flash-confirm', { btn: this.$t('OK') }) }}</p>
      <el-text type="info">{{ this.$t('Size') + ': ' + bytesToHuman(this.size) }}</el-text>
      <el-text type="info">{{ 'MD5: ' + this.md5 }}</el-text>
      <el-checkbox v-model="keepConfig" :label="$t('Keep settings and retain the current configuration')"/>
    </el-space>
    <template #footer>
      <el-button @click="modalConfirm = false">{{  $t('Cancel') }}</el-button>
      <el-button type="primary" @click="doUpgrade">{{ $t('OK') }}</el-button>
    </template>
  </el-dialog>
</template>

<script>
export default {
  data() {
    return {
      loading: false,
      modalConfirm: false,
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
    onUploadSuccess(res) {
      this.loading = false

      this.$oui.ubus('system', 'validate_firmware_image', { path: '/tmp/firmware.bin' }).then(({ valid }) => {
        if (!valid) {
          this.$message.error(this.$t('Firmware verification failed. Please upload the firmware again'))
        } else {
          this.size = res.size
          this.md5 = res.md5
          this.modalConfirm = true
          this.keepConfig = true
        }
      })
    },
    doUpgrade() {
      this.modalConfirm = false

      this.$oui.call('system', 'sysupgrade', { keep: this.keepConfig }).then(() => {
        const loading = this.$loading({
          lock: true,
          text: this.$t('Upgrading') + '...',
          background: 'rgba(0, 0, 0, 0.7)'
        })

        this.$oui.reconnect().then(() => {
          loading.close()
          this.$router.push('/login')
        })
      })
    }
  }
}
</script>

<i18n src="./locale.json"/>
