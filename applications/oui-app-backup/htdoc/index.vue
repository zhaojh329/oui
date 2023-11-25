<template>
  <div v-if="backupUrl" class="backup">
    <el-link :href="backupUrl" download="backup.tar.gz">backup.tar.gz</el-link>
  </div>
  <el-button type="primary" style="margin-top: 4px" @click="generateBackup">{{ $t('Generate backup file') }}</el-button>
  <el-divider/>
  <el-text size="large">{{ $t('Restore from backup') }}</el-text>
  <el-upload v-loading="loading" drag action="/oui-upload" :data="{sid: $oui.state.sid, path: '/tmp/backup.tar.gz'}"
    :show-file-list="false" :on-success="onUploadSuccess" :before-upload="() => loading = true">
    <el-icon size="30"><upload-filled/></el-icon>
    <div>{{ $t('Click or drag files to this area to upload') }}</div>
  </el-upload>
  <el-dialog v-model="modalConfirm" :title="$t('Apply backup') + '?'">
    <el-space direction="vertical" fill>
      <p>{{ $t('restore-confirm', [ this.$t('Continue'), this.$t('Cancel') ]) }}</p>
      <el-input readonly type="textarea" :autosize="{minRows: 5, maxRows: 10}" :value="backupFiles"/>
    </el-space>
    <template #footer>
      <el-button @click="modalConfirm = false">{{  $t('Cancel') }}</el-button>
      <el-button type="primary" @click="doRestore">{{ $t('Continue') }}</el-button>
    </template>
  </el-dialog>
  <el-divider/>
  <el-button type="danger" @click="doReset">{{ $t('Perform reset') }}</el-button>
</template>

<script>
export default {
  data() {
    return {
      backupUrl: '',
      backupFiles: '',
      loading: false,
      modalConfirm: false
    }
  },
  methods: {
    generateBackup() {
      this.loading = true
      const sid = this.$oui.state.sid
      this.$oui.call('system', 'create_backup', { path: '/tmp/backup.tar.gz' }).then(() => {
        this.axios.get(`/oui-download?sid=${sid}&path=/tmp/backup.tar.gz`, { responseType: 'blob' }).then(resp => {
          this.backupUrl = window.URL.createObjectURL(resp.data)
          this.loading = false
        })
      })
    },
    onUploadSuccess() {
      this.loading = false

      this.$oui.call('system', 'list_backup', { path: '/tmp/backup.tar.gz' }).then(({ files }) => {
        if (!files) {
          this.$message.error(this.$t('The uploaded backup archive is not readable'))
        } else {
          this.backupFiles = files
          this.modalConfirm = true
        }
      })
    },
    doRestore() {
      this.modalConfirm = false

      this.$oui.call('system', 'restore_backup', { path: '/tmp/backup.tar.gz' }).then(() => {
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
    },
    doReset() {
      this.$confirm(this.$t('ResettConfirm') + '?', this.$t('Reset to defaults'), {
        type: 'warning'
      }).then(() => {
        this.$oui.ubus('system', 'reset').then(() => {
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
      })
    }
  }
}
</script>

<style scoped>
.backup a {
  font-size: 1.5em;
  text-decoration: none;
  margin-right: 10px;
}

.backup:hover {
  text-decoration: underline;
}
</style>

<i18n src="./locale.json"/>
