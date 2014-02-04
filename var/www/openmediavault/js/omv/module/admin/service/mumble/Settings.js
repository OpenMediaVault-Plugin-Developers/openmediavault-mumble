/**
 * Copyright (C) 2014 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")

Ext.define("OMV.module.admin.service.mumble.Settings", {
    extend : "OMV.workspace.form.Panel",

    rpcService   : "Mumble",
    rpcGetMethod : "getSettings",
    rpcSetMethod : "setSettings",

    getFormItems : function() {
        var me = this;

        return [{
            xtype    : "fieldset",
            title    : _("General settings"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype      : "checkbox",
                name       : "enable",
                fieldLabel : _("Enable"),
                checked    : false
            },{
                xtype      : "textfield",
                name       : "serverpassword",
                inputType  : "password",
                fieldLabel : _("Server password")
            },{
                xtype         : "numberfield",
                name          : "bandwidth",
                fieldLabel    : _("Bandwidth"),
                minValue      : 18200,
                maxValue      : 130000,
                allowDecimals : false,
                allowNegative : false,
                allowBlank    : false,
                value         : 72000,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("Per-user max bandwidth usage in bits per second.")
                }]
            },{
                xtype         : "numberfield",
                name          : "users",
                fieldLabel    : _("Maximum users"),
                minValue      : 0,
                allowDecimals : false,
                allowNegative : false,
                allowBlank    : false,
                value         : 100
            },{
                xtype      : "checkbox",
                name       : "force_opus",
                fieldLabel : _("Force Opus codec"),
                value      : false
            }]
        },{
            xtype    : "fieldset",
            title    : _("Server registration"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype      : "textfield",
                name       : "register_name",
                fieldLabel : _("Name")
            }]
        },{
            xtype    : "fieldset",
            title    : _("SuperUser"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype       : "textfield",
                name        : "superuser_password",
                inputType   : "password",
                isFormField : false,
                fieldLabel  : _("SuperUser password")
            },{
                xtype      : "button",
                text       : _("Save password"),
                handler    : function() {
                    var field = me.down("field[name=superuser_password]");

                    OMV.Rpc.request({
                        scope       : me,
                        relayErrors : true,
                        callback    : function(id, success, response) {
                            if (!success) {
                                OMV.MessageBox.error(null, response);
                            } else {
                                field.setValue("");
                                OMV.MessageBox.success(_("Password changed!"), _("The password was successfully changed."));
                            }
                        },
                        rpcData : {
                            service : "Mumble",
                            method  : "setSuperUserPassword",
                            params  : {
                                password : field.getValue()
                            }
                        }
                    });
                },
                margin : "0 0 5 0"
            }]
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "settings",
    path      : "/service/mumble",
    text      : _("Settings"),
    position  : 10,
    className : "OMV.module.admin.service.mumble.Settings"
});
