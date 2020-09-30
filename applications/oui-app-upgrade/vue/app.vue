<template>
  <div>
    <a-card :title="$t('Backup / Restore')" style="margin-bottom: 15px">
      <form ref="backup" method="POST" action="/download">
        <input v-show="false" type="text" value="/tmp/backup.tar.gz" name="path"/>
        <input v-show="false" type="text" value="backup.tar.gz" name="filename"/>
      </form>
      <p style="margin-bottom: 10px">{{ $t('upgrade.BackupDesc') }}</p>
      <a-button type="primary" size="small" @click="generateArchive" style="margin-right: 10px">{{ $t('upgrade.Generate archive') }}</a-button>
      <a-button type="danger" size="small" @click="performReset">{{ $t('upgrade.Perform reset') }}</a-button>
      <p style="margin-bottom: 10px; margin-top: 10px">{{ $t('upgrade.RestoreDesc') }}</p>
      <a-upload action="/upload" @change="onUploadArchive" :data="{path: '/tmp/backup.tar.gz'}">
        <a-button size="small" type="primary" icon="upload">{{ $t('upgrade.Select File') }}</a-button>
      </a-upload>
    </a-card>
    <a-card :title="$t('upgrade.Flash new firmware image')">
      <p style="margin-bottom: 10px">{{ $t('upgrade.UpgradeDesc') }}</p>
      <a-upload action="/upload" @change="onUploadFirmware" :data="{path: '/tmp/firmware.bin'}">
        <a-button size="small" type="primary" icon="upload">{{ $t('upgrade.Select File') }}</a-button>
      </a-upload>
    </a-card>
  </div>
</template>

<script>
export default {
  name: 'upgrade',
  methods: {
    restoreBackup () {
      return this.$rpc.call('upgrade', 'backup_restore', { path: '/tmp/backup.tar.gz' })
    },
    testUpgrade () {
      return this.$rpc.call('upgrade', 'verify', { path: '/tmp/firmware.bin' })
    },
    startUpgrade (keep) {
      return this.$rpc.call('upgrade', 'upgrade', { keep, path: '/tmp/firmware.bin' })
    },
    cleanUpgrade () {
      return this.$rpc.call('file', 'remove', { path: '/tmp/firmware.bin' })
    },
    cleanBackup () {
      return this.$rpc.call('file', 'remove', { path: '/tmp/backup.tar.gz' })
    },
    generateArchive () {
      this.$rpc.call('upgrade', 'backup', { path: '/tmp/backup.tar.gz' }).then(res => {
        if (res.code === 0) {
          this.$refs.backup.submit()
        } else {
          this.$message.error('Backup fail')
        }
      })
    },
    performReset () {
      this.$confirm({
        title: this.$t('upgrade.Really reset all changes?'),
        content: this.$t('upgrade.PerformResetContent'),
        onOk: () => {
          this.$rpc.call('system', 'factory').then(() => {
            this.$reconnect(this.$t('Rebooting...'))
          })
        }
      })
    },
    renderFileInfo (h, title, size, md5, isFirmware) {
      return h('div', [
        h('p', title),
        h('ul', [
          h('li', [
            h('strong', ['MD5: ']),
            md5
          ]),
          h('li', [
            h('strong', [this.$t('upgrade.Size') + ': ']),
            size
          ])
        ]),
        isFirmware && h('input', {
          attrs: {
            id: 'upgrade-firmware-keep',
            type: 'checkbox',
            checked: true
          }
        }),
        isFirmware && ' ' + this.$t('upgrade.Keep configuration when reflashing')
      ])
    },
    onUploadArchive (info) {
      const file = info.file
      const status = file.status

      if (status === 'uploading' || status === 'removed') return

      info.fileList.length = 0

      if (status !== 'done') {
        this.$message.error(`upload '${file.name}' failed.`)
        return
      }

      const title = this.$t('upgrade.Backup restore')

      this.$rpc.call('file', 'md5sum', { path: '/tmp/backup.tar.gz' }).then(res => {
        this.$confirm({
          title,
          width: 450,
          content: h => this.renderFileInfo(h, this.$t('upgrade.backup archive was uploaded completely', { btn: this.$t('upgrade.OK') }), (file.size / 1024).toFixed(2) + ' KB', res.md5),
          onOk: () => {
            this.restoreBackup().then(r => {
              if (r.code === 0) {
                this.$confirm({
                  title,
                  content: this.$t('upgrade.RestoreConfirm'),
                  okText: this.$t('upgrade.Reboot now'),
                  onOk: () => {
                    this.$system.reboot().then(() => {
                      this.$reconnect(this.$t('Rebooting...'))
                    })
                  }
                })
              } else {
                this.$info({
                  title,
                  content: this.$t('upgrade.RestoreFail'),
                  okText: this.$t('upgrade.Close'),
                  onOk: () => this.cleanBackup()
                })
              }
            })
          },
          onCancel: () => this.cleanBackup()
        })
      })
    },
    onUploadFirmware (info) {
      const file = info.file
      const status = file.status

      if (status === 'uploading' || status === 'removed') return

      info.fileList.length = 0

      if (status !== 'done') {
        this.$message.error(`upload '${file.name}' failed.`)
        return
      }

      this.testUpgrade().then(res => {
        if (res.code === 0) {
          const title = this.$t('upgrade.Verify firmware')

          this.$confirm({
            title,
            width: 450,
            content: h => this.renderFileInfo(h, this.$t('upgrade.The firmware image was uploaded completely', { btn: this.$t('upgrade.OK') }), (file.size / 1024 / 1024).toFixed(2) + ' MB', res.md5, true),
            onOk: () => {
              const keep = document.getElementById('upgrade-firmware-keep').checked
              this.startUpgrade(keep).then(() => {
                this.$reconnect(this.$t('upgrade.Upgrading...'))
              })
            },
            onCancel: () => this.cleanUpgrade()
          })
        } else {
          this.$error({
            title: this.$t('upgrade.Invalid image'),
            content: this.$t('upgrade.InvalidImageContent'),
            okText: this.$t('upgrade.Close'),
            onOk: () => this.cleanUpgrade()
          })
        }
      })
    }
  }
}
</script>
