<template>
  <v-dialog v-model="show" :persistent="rebooting" width="320">
    <v-card :color="rebooting ? 'primary' : ''">
      <v-card-text>
        <div v-if="rebooting">
          Rebooting...
          <v-progress-linear indeterminate color="white" class="mb-0"></v-progress-linear>
        </div>
        <span class="red--text" v-else>Are you sure you want to restart your device?</span>
      </v-card-text>
      <v-divider></v-divider>
      <v-card-actions v-if="!rebooting">
        <v-spacer></v-spacer>
        <v-btn color="primary" flat @click="reboot">Perform Reboot</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
  export default {
    name: 'reboot-dialog',
    data() {
      return {
        show: true,
        rebooting: false
      }
    },
    watch: {
      show() {
        this.$emit('close');
      }
    },
    methods: {
      reboot() {
        this.$ubus.call('rpc-sys', 'reboot').then(() => {
          this.rebooting = true;  
          sessionStorage.removeItem('_ubus_rpc_session');

          window.setTimeout(() => {
            let interval = window.setInterval(() => {
              this.axios.get('/').then(() => {
                this.show = false;
                this.$emit('close');
                window.clearInterval(interval);
                window.location.href = '/';
              });
            });
          }, 5000);
        });
      }
    }
  };
</script>
