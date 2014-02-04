<?php

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

require_once "openmediavault/object.inc";
require_once "openmediavault/error.inc";
require_once "openmediavault/util.inc";
require_once "openmediavault/rpc.inc";
require_once "openmediavault/notify.inc";

class OMVRpcServiceMumble extends OMVRpcServiceAbstract
{
    private function getXPath()
    {
        return "//services/mumble";
    }

    public function getName()
    {
        return "Mumble";
    }

    public function initialize()
    {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");
        $this->registerMethod("setSuperUserPassword");
    }

    public function getSettings($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Get configuration object
        $object = $xmlConfig->get($this->getXPath());
        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath());

        // Modify result data
        $settingsObject["enable"]         = boolval($object["enable"]);
        $settingsObject["bandwidth"]      = intval($object["bandwidth"]);
        $settingsObject["users"]          = intval($object["users"]);
        $settingsObject["force_opus"]     = boolval($object["force_opus"]);
        $settingsObject["register_name"]  = $object["register_name"];

        return $settingsObject;
    }

    public function setSettings($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "enable"         : { "type" : "boolean" },
                "serverpassword" : { "type" : "string" },
                "bandwidth"      : { "type" : "integer" },
                "users"          : { "type" : "integer" },
                "force_opus"     : { "type" : "boolean" },
                "register_name"  : { "type" : "string" }
            }
        }');

        // Prepare configuration data
        $object = array(
            "enable"         => array_boolval($params, "enable"),
            "serverpassword" => $params["serverpassword"],
            "bandwidth"      => intval($params["bandwidth"]),
            "users"          => intval($params["users"]),
            "force_opus"     => array_boolval($params, "force_opus"),
            "register_name"  => $params["register_name"]
        );

        // Set configuration object
        if (false === $xmlConfig->replace($this->getXPath(), $object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED,
                $this->$xPath);

        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY, "org.openmediavault.services.mumble", $object);

        return $object;
    }

    public function setSuperUserPassword($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "password" : { "type" : "string" }
            }
        }');

        $password = trim($params["password"]);

        if (strlen($password) < 8)
            throw new \Exception("The password length should be at least eight characters long.");

        $cmd = sprintf("sudo -i murmurd -ini /etc/mumble-server.ini -supw %s", $password);

        if (0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
                $cmd, implode("\n", $output));
    }
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceMumble());
