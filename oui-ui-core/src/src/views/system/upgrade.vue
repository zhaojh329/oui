<template>
  <div>
    <a-card :title="$t('Backup / Restore')" style="margin-bottom: 15px">
      <form ref="backup" method="POST" action="/cgi-bin/cgi-backup">
        <input v-show="false" type="text" :value="sid" name="sessionid"/>
      </form>
      <p style="margin-bottom: 10px">{{ $t('Backup-Restore-desc') }}</p>
      <a-button type="primary" size="small" @click="generateArchive" style="margin-right: 10px">{{ $t('Generate archive') }}</a-button>
      <a-button type="danger" size="small" @click="performReset">{{ $t('Perform reset') }}</a-button>
      <p style="margin-bottom: 10px; margin-top: 10px">{{ $t('To restore configuration files, you can upload a previously generated backup archive here.') }}</p>
      <a-upload action="/cgi-bin/cgi-upload" @change="onUploadArchive" :data="{filename: '/tmp/backup.tar.gz', sessionid: sid}">
        <a-button size="small" type="primary" icon="upload">{{ $t('Select File') }}</a-button>
      </a-upload>
    </a-card>
    <a-card :title="$t('Flash new firmware image')">
      <p style="margin-bottom: 10px">{{ $t('Upgrade-Firmware-desc') }}</p>
      <a-upload action="/cgi-bin/cgi-upload" @change="onUploadFirmware" :data="{filename: '/tmp/firmware.bin', sessionid: sid}">
        <a-button size="small" type="primary" icon="upload">{{ $t('Select File') }}</a-button>
      </a-upload>
    </a-card>
  </div>
</template>

<script>
export default {
  name: 'upgrade',
  data () {
    return {
      sid: ''
    }
  },
  methods: {
    restoreBackup () {
      return this.$ubus.call('oui.system', 'backup_restore')
    },
    testUpgrade () {
      return this.$ubus.call('rpc-sys', 'upgrade_test')
    },
    startUpgrade (keep) {
      return this.$ubus.call('rpc-sys', 'upgrade_start', { keep })
    },
    cleanUpgrade () {
      return this.$ubus.call('rpc-sys', 'upgrade_clean')
    },
    cleanBackup () {
      return this.$ubus.call('oui.system', 'backup_clean')
    },
    generateArchive () {
      this.$refs.backup.submit()
    },
    performReset () {
      this.$confirm({
        title: this.$t('Really reset all changes?'),
        content: this.$t('This will reset the system to its initial configuration, all changes made since the initial flash will be lost!'),
        onOk: () => {
          this.$ubus.call('rpc-sys', 'factory').then(() => {
            this.$reconnect(this.$t('Rebooting...'))
          })
        }
      })
    },
    renderFileInfo (h, title, size, checksum, isFirmware) {
      // content += `<input id="upgrade-firmware-keep" type="checkbox" checked> ${this.$t('Keep configuration when reflashing')}`
      return h('div', [
        h('p', title),
        h('ul', [
          h('li', [
            h('strong', [this.$t('Checksum') + ': ']),
            checksum
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

      this.$confirm({
        title,
        width: 450,
        content: h => this.renderFileInfo(h, this.$t('backup archive was uploaded completely', { btn: this.$t('OK') }), (file.size / 1024).toFixed(2) + ' KB', file.response.checksum),
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
            content: h => this.renderFileInfo(h, this.$t('The firmware image was uploaded completely', { btn: this.$t('OK') }), (file.size / 1024 / 1024).toFixed(2) + ' MB', file.response.checksum, true),
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
  },
  created () {
    this.sid = this.$session.sid()
  }
}
</script>
