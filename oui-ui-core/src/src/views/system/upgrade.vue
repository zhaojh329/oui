<template>
  <div>
    <el-card :header="$t('Backup / Restore')" style="margin-bottom: 15px">
      <form ref="backup" method="POST" action="/cgi-bin/cgi-backup">
        <input v-show="false" type="text" :value="sid" name="sessionid" />
      </form>
      <p>{{ $t('Backup-Restore-desc') }}</p>
      <el-button type="primary" size="small" @click="generateArchive">{{ $t('Generate archive') }}</el-button>
      <el-button type="danger" size="small" @click="performReset">{{ $t('Perform reset') }}</el-button>
      <p>{{ $t('To restore configuration files, you can upload a previously generated backup archive here.') }}</p>
      <el-upload ref="archive" action="/cgi-bin/cgi-upload" :on-success="onUploadArchiveSuccess" :file-list="fileListArchive" :auto-upload="false" :limit="1" :data="{filename: '/tmp/backup.tar.gz', sessionid: sid}" style="width: 600px">
        <el-button slot="trigger" size="small" type="primary">{{ $t('Select File') }}</el-button>
        <el-button style="margin-left: 10px;" size="small" type="success" @click="uploadFile('archive')">{{ $t('Upload archive...') }}</el-button>
      </el-upload>
    </el-card>
    <el-card :header="$t('Flash new firmware image')">
      <p>{{ $t('Upgrade-Firmware-desc') }}</p>
      <el-upload ref="firmware" action="/cgi-bin/cgi-upload" :on-success="onUploadFirmwareSuccess" :file-list="fileListFirmware" :auto-upload="false" :limit="1" :data="{filename: '/tmp/firmware.bin', sessionid: sid}" style="width: 600px">
        <el-button slot="trigger" size="small" type="primary">{{ $t('Select File') }}</el-button>
        <el-button style="margin-left: 10px;" size="small" type="success" @click="uploadFile('firmware')">{{ $t('Upload Firmware...') }}</el-button>
      </el-upload>
    </el-card>
  </div>
</template>

<script>
export default {
  name: 'upgrade',
  data() {
    return {
      sid: '',
      fileListArchive: [],
      fileListFirmware: []
    }
  },
  methods: {
    restoreBackup() {
      return this.$ubus.call('oui.system', 'backup_restore');
    },
    testUpgrade() {
      return this.$ubus.call('rpc-sys', 'upgrade_test');
    },
    startUpgrade(keep) {
      return this.$ubus.call('rpc-sys', 'upgrade_start', {keep});
    },
    cleanUpgrade() {
      return this.$ubus.call('rpc-sys', 'upgrade_clean');
    },
    cleanBackup() {
      return this.$ubus.call('oui.system', 'backup_clean');
    },
    generateArchive() {
      this.$refs['backup'].submit();
    },
    performReset() {
      this.$confirm(this.$t('This will reset the system to its initial configuration, all changes made since the initial flash will be lost!'), this.$t('Really reset all changes?')).then(() => {
        this.$ubus.call('rpc-sys', 'factory').then(() => {
          this.$reconnect(this.$t('Rebooting...'));
        });
      });
    },
    uploadFile(ref) {
      this.$refs[ref].submit();
    },
    onUploadArchiveSuccess(res) {
      let title = this.$t('Backup restore');
      let content = '<p>' + this.$t('backup archive was uploaded completely', {btn: this.$t('OK')}) + '</p>';
      content += '<ul>';
      content += `<li><strong>${this.$t('Checksum')}: </strong>${res.checksum}</li>`;

      const size = (res.size / 1024).toFixed(2);
      content += `<li><strong>${this.$t('Size')}: </strong>${size} KB</li>`;
      content += '</ul>';

      this.$confirm(content, title, {
        dangerouslyUseHTMLString: true
      }).then(() => {
        this.restoreBackup().then(r => {
          if (r.code === 0) {
            content = this.$t('The backup was successfully restored, it is advised to reboot the system now in order to apply all configuration changes.');
            this.$confirm(content, title, {
              confirmButtonText: this.$t('Reboot now')
            }).then(() => {
              this.$system.reboot().then(() => {
                this.$reconnect(this.$t('Rebooting...'));
              });
            });
          } else {
            content = this.$t('Backup restoration failed, Make sure that you choose the file format for your platform.');
            this.$confirm(content, title, {
              showCancelButton: false,
              confirmButtonText: this.$t('Close')
            }).then(() => {
              this.cleanBackup();
            });
          }
        });
      });
    },
    onUploadFirmwareSuccess(info) {
      this.testUpgrade().then(res => {
        if (res.code === 0) {
          let title = this.$t('Verify firmware');
          let content = '<p>' + this.$t('The firmware image was uploaded completely', {btn: this.$t('OK')}) + '</p>'
          content += '<ul>';
          content += `<li><strong>${this.$t('Checksum')}: </strong>${info.checksum}</li>`;

          const size = (info.size / 1024 / 1024).toFixed(2);
          content += `<li><strong>${this.$t('Size')}: </strong>${size} MB</li>`;
          content += '</ul>';
          content += `<input id="upgrade-firmware-keep" type="checkbox" checked> ${this.$t('Keep configuration when reflashing')}`;

          this.$confirm(content, title, {
            showClose: false,
            closeOnClickModal: false,
            dangerouslyUseHTMLString: true
          }).then(() => {
            const keep = document.getElementById('upgrade-firmware-keep').checked;
            this.startUpgrade(keep).then(() => {
              this.$reconnect(this.$t('Upgrading...'));
            });
          }).catch(() => {
            this.cleanUpgrade();
          });
        } else {
          const content = this.$t('The uploaded image file does not contain a supported format. Make sure that you choose the generic image format for your platform.');
          this.$confirm(content, this.$t('Invalid image'), {
            showCancelButton: false,
            confirmButtonText: this.$t('Close')
          }).then(() => {
            this.cleanUpgrade();
          });
        }
      });
    }
  },
  created() {
    this.sid = this.$session.sid();
  }
}
</script>
