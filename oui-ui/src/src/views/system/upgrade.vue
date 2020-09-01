<template>
  <div>
    <a-card :title="$t('Backup / Restore')" style="margin-bottom: 15px">
      <form ref="backup" method="POST" action="/download">
        <input v-show="false" type="text" value="/tmp/backup.tar.gz" name="path"/>
        <input v-show="false" type="text" value="backup.tar.gz" name="filename"/>
      </form>
      <p style="margin-bottom: 10px">{{ $t('Backup-Restore-desc') }}</p>
      <a-button type="primary" size="small" @click="generateArchive" style="margin-right: 10px">{{ $t('Generate archive') }}</a-button>
      <a-button type="danger" size="small" @click="performReset">{{ $t('Perform reset') }}</a-button>
      <p style="margin-bottom: 10px; margin-top: 10px">{{ $t('To restore configuration files, you can upload a previously generated backup archive here.') }}</p>
      <a-upload action="/upload" @change="onUploadArchive" :data="{path: '/tmp/backup.tar.gz'}">
        <a-button size="small" type="primary" icon="upload">{{ $t('Select File') }}</a-button>
      </a-upload>
    </a-card>
    <a-card :title="$t('Flash new firmware image')">
      <p style="margin-bottom: 10px">{{ $t('Upgrade-Firmware-desc') }}</p>
      <a-upload action="/upload" @change="onUploadFirmware" :data="{path: '/tmp/firmware.bin'}">
        <a-button size="small" type="primary" icon="upload">{{ $t('Select File') }}</a-button>
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
        title: this.$t('Really reset all changes?'),
        content: this.$t('This will reset the system to its initial configuration, all changes made since the initial flash will be lost!'),
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
            h('strong', [this.$t('Size') + ': ']),
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
        isFirmware && ' ' + this.$t('Keep configuration when reflashing')
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

      const title = this.$t('Backup restore')

      this.$rpc.call('file', 'md5sum', { path: '/tmp/backup.tar.gz' }).then(res => {
        this.$confirm({
          title,
          width: 450,
          content: h => this.renderFileInfo(h, this.$t('backup archive was uploaded completely', { btn: this.$t('OK') }), (file.size / 1024).toFixed(2) + ' KB', res.md5),
          onOk: () => {
            this.restoreBackup().then(r => {
              if (r.code === 0) {
                this.$confirm({
                  title,
                  content: this.$t('The backup was successfully restored, it is advised to reboot the system now in order to apply all configuration changes.'),
                  okText: this.$t('Reboot now'),
                  onOk: () => {
                    this.$system.reboot().then(() => {
                      this.$reconnect(this.$t('Rebooting...'))
                    })
                  }
                })
              } else {
                this.$info({
                  title,
                  content: this.$t('Backup restoration failed, Make sure that you choose the file format for your platform.'),
                  okText: this.$t('Close'),
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
          const title = this.$t('Verify firmware')

          this.$confirm({
            title,
            width: 450,
            content: h => this.renderFileInfo(h, this.$t('The firmware image was uploaded completely', { btn: this.$t('OK') }), (file.size / 1024 / 1024).toFixed(2) + ' MB', res.md5, true),
            onOk: () => {
              const keep = document.getElementById('upgrade-firmware-keep').checked
              this.startUpgrade(keep).then(() => {
                this.$reconnect(this.$t('Upgrading...'))
              })
            },
            onCancel: () => this.cleanUpgrade()
          })
        } else {
          this.$error({
            title: this.$t('Invalid image'),
            content: this.$t('The uploaded image file does not contain a supported format. Make sure that you choose the generic image format for your platform.'),
            okText: this.$t('Close'),
            onOk: () => this.cleanUpgrade()
          })
        }
      })
    }
  }
}
</script>
