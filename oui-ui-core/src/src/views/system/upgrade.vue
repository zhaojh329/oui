<template>
  <div>
    <Card title="Backup" style="margin-bottom: 15px">
      <form ref="form" method="POST" action="/cgi-bin/oui-backup">
        <input v-show="false" type="text" v-model="sid" name="sessionid" />
      </Form>
      <Button type="primary" @click="handleSubmit">Generate archive</Button>
    </Card>
    <Card title="Restore" style="margin-bottom: 15px">
      <Button type="error" @click="handleReset" style="margin-bottom: 15px">Perform Reset</Button>
      <br />
      <Upload ref="upload" action="/cgi-bin/oui-upload" :before-upload="beforeUpload" :on-success="onUpSsuccess" :data="uploadData">
        <Button icon="ios-cloud-upload-outline">Upload archive</Button>
      </Upload>
      <div v-if="file !== null">
        {{ file.name }}
        <Button type="text" @click="uploadArchive" :loading="loadingStatus">{{ loadingStatus ? 'Uploading' : 'Click to upload' }}</Button>
      </div>
    </Card>
  </div>
</template>

<script>

export default {
  name: 'upgrade',
  data() {
    return {
      sid: '',
      file: null,
      loadingStatus: false,
      uploadData: {
        filename: '/tmp/backup.tar.gz',
        sessionid: ''
      }
    }
  },
  methods: {
    showLoading(title) {
      this.$Spin.show({
        render: (h) => {
          return h('div', [
            h('Icon', {
              'class': 'demo-spin-icon-load',
              props: {
                type: 'ios-loading',
                size: 18
              }
            }),
            h('div', title)
          ])
        }
      });
    },
    handleSubmit() {
      this.$refs.form.submit();
    },
    reconnect(title) {
      this.showLoading(title);
      window.setTimeout(() => {
        const interval = window.setInterval(() => {
          this.axios.get('/favicon.ico').then(() => {
            this.$Spin.hide();
            window.clearInterval(interval);
            this.$router.push('/login');
          }).catch(() => {});
        }, 5000);
      }, 5000);
    },
    handleReset() {
      this.$Modal.confirm({
        content: 'Are you sure you want to restore your device to the factory?',
        onOk: () => {
          this.$ubus.call('rpc-sys', 'factory').then(() => {
            this.reconnect('Rebooting...');
          });
        }
      });
    },
    beforeUpload(file) {
      this.file = file;
      return false;
    },
    uploadArchive() {
      this.$refs.upload.post(this.file);
    },
    onUpSsuccess(r) {
      console.log(r);
      this.$Modal.confirm({
        content: 'Upload archive OK. Whether to restart the device immediately for the configuration to take effect?',
        onOk: () => {
          this.$ubus.call('rpc-sys', 'reboot').then(() => {
            this.reconnect('Rebooting...');
          });
        }
      });
    }
  },
  mounted() {
    this.sid = this.$session.sid();
    this.uploadData.sessionid = this.sid;
  }
}
</script>

<style>
  .demo-spin-icon-load{
    animation: ani-demo-spin 1s linear infinite;
  }
</style>
