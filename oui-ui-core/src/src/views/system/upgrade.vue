<template>
  <div>
    <el-card header="Backup / Restore" style="margin-bottom: 15px">
      <form ref="backup" method="POST" action="/cgi-bin/oui-backup">
        <input v-show="false" type="text" :value="sid" name="sessionid" />
      </form>
      <p>Click "Generate archive" to download a tar archive of the current configuration files. To reset the firmware to its initial state, click "Perform reset" (only possible with squashfs images).</p>
      <el-button type="primary" @click="generateArchive">Generate archive</el-button>
      <el-button type="danger" @click="performReset">Perform reset</el-button>
      <p>To restore configuration files, you can upload a previously generated backup archive here.</p>
      <el-upload ref="archive" action="/cgi-bin/oui-upload" :on-success="onUploadArchiveSuccess" :file-list="fileListArchive" :auto-upload="false" :limit="1" :data="{filename: '/tmp/backup.tar.gz', sessionid: sid}" style="width: 600px">
        <el-button slot="trigger" size="small" type="primary">Select File</el-button>
        <el-button style="margin-left: 10px;" size="small" type="success" @click="uploadFile('archive')">Upload archive...</el-button>
      </el-upload>
    </el-card>
    <el-card header="Flash new firmware image">
      <p>Upload a sysupgrade-compatible image here to replace the running firmware. Check "Keep settings" to retain the current configuration (requires an OpenWrt compatible firmware image).</p>
      <el-upload ref="firmware" action="/cgi-bin/oui-upload" :on-success="onUploadFirmwareSuccess" :file-list="fileListFirmware" :auto-upload="false" :limit="1" :data="{filename: '/tmp/firmware.bin', sessionid: sid}" style="width: 600px">
        <el-button slot="trigger" size="small" type="primary">Select File</el-button>
        <el-button style="margin-left: 10px;" size="small" type="success" @click="uploadFile('firmware')">Upload Firmware...</el-button>
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
      this.$confirm('This will reset the system to its initial configuration, all changes made since the initial flash will be lost!', 'Really reset all changes?').then(() => {
        this.$ubus.call('rpc-sys', 'factory').then(() => {
          this.$reconnect('Rebooting...');
        });
      });
    },
    uploadFile(ref) {
      this.$refs[ref].submit();
    },
    onUploadArchiveSuccess(res) {
      let title = 'Backup restore';
      let content = '<p>The backup archive was uploaded completely. Please verify the checksum and file size below, then click "Ok" to restore the archive.</p>'
      content += '<ul>';
      content += `<li><strong>Checksum: </strong>${res.checksum}</li>`;

      const size = (res.size / 1024).toFixed(2);
      content += `<li><strong>Size: </strong>${size} KB</li>`;
      content += '</ul>';

      this.$confirm(content, title, {
        dangerouslyUseHTMLString: true
      }).then(() => {
        this.restoreBackup().then(r => {
          if (r.code === 0) {
            content = 'The backup was successfully restored, it is advised to reboot the system now in order to apply all configuration changes.';
            this.$confirm(content, title, {
              confirmButtonText: 'Reboot now'
            }).then(() => {
              this.$system.reboot().then(() => {
                this.$reconnect('Rebooting...');
              });
            });
          } else {
            content = 'Backup restoration failed, Make sure that you choose the file format for your platform.';
            this.$confirm(content, title, {
              showCancelButton: false,
              confirmButtonText: 'Close'
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
          let title = 'Verify firmware';
          let content = '<p>The firmware image was uploaded completely. Please verify the checksum and file size below, then click "Ok" to start the flash procedure.</p>'
          content += '<ul>';
          content += `<li><strong>Checksum: </strong>${info.checksum}</li>`;

          const size = (info.size / 1024 / 1024).toFixed(2);
          content += `<li><strong>Size: </strong>${size} MB</li>`;
          content += '</ul>';
          content += '<input id="upgrade-firmware-keep" type="checkbox" checked> Keep configuration when reflashing';

          this.$confirm(content, title, {
            dangerouslyUseHTMLString: true
          }).then(() => {
            const keep = document.getElementById('upgrade-firmware-keep').checked;
            this.startUpgrade(keep).then(() => {
              this.$reconnect('upgrading...');
            });
          });
        } else {
          const content = 'The uploaded image file does not contain a supported format. Make sure that you choose the generic image format for your platform.';
          this.$confirm(content, 'Invalid image', {
            showCancelButton: false,
            confirmButtonText: 'Close'
          }).then(() => {
            this.cleanUpgrade();
          });
        }
      });
    }
  },
  mounted() {
    this.sid = this.$session.sid();
  }
}
</script>
