<template>
  <v-app>
    <v-content>
      <v-container fluid fill-height>
        <v-layout align-center justify-center>
          <v-flex xs12 sm8 md4>
            <v-alert type="error" :value="!login">Invalid username and/or password! Please try again.</v-alert>
            <v-card class="elevation-12">
              <v-toolbar dark color="primary">
                <v-toolbar-title>Authorization Required</v-toolbar-title>
              </v-toolbar>
              <v-card-text>
                <p>Please enter your username and password.</p>
                <v-form ref="form">
                  <v-text-field prepend-icon="person" v-model="username" :rules="rules.username" label="Username" type="text"></v-text-field>
                  <v-text-field prepend-icon="lock" v-model="password" label="Password" type="password" @keyup.enter="submit"></v-text-field>
                </v-form>
              </v-card-text>
              <v-card-actions>
                <v-spacer></v-spacer>
                <v-btn color="primary" @click="submit">Login</v-btn>
                <v-btn @click="reset">Reset</v-btn>
              </v-card-actions>
            </v-card>
          </v-flex>
        </v-layout>
      </v-container>
    </v-content>
  </v-app>
</template>

<script>
  export default {
    data: () => ({
      login: true,
      username: '',
      password: '',
      rules: {
        username: [v => !!v || 'Username is required']
      }
    }),
    methods: {
      submit() {
        if (this.$refs.form.validate()) {
          this.$session.login(this.username, this.password).then(() => {
            this.$router.push('/');
          }).catch(() => {
            this.login = false;
          });
        }
      },
      reset() {
        this.login = true;
        this.$refs.form.reset();
      }
    }
  };
</script>
