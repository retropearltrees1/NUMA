import { useEffect, useRef } from "react";
import type { Message } from "../types";

interface MessageListProps {
  messages: Message[];
  isTyping: boolean;
}

function formatTime(ts: number): string {
  const d = new Date(ts);
  return d.toLocaleTimeString("fr-FR", { hour: "2-digit", minute: "2-digit" });
}

function renderText(text: string): React.ReactNode {
  return text.split("\n").map((line, i) => (
    <span key={i}>
      {line}
      {i < text.split("\n").length - 1 && <br />}
    </span>
  ));
}

export default function MessageList({ messages, isTyping }: MessageListProps) {
  const endRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    endRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages, isTyping]);

  return (
    <div className="message-list">
      {messages.map((msg) => (
        <div key={msg.id} className={`message-row ${msg.role === "user" ? "message-user" : "message-numa"}`}>
          <div className={`message-avatar ${msg.role === "user" ? "avatar-user" : "avatar-numa"}`}>
            {msg.role === "user" ? "Toi" : "N"}
          </div>
          <div className="message-content">
            <div className="message-meta">
              <span className="message-sender">{msg.role === "user" ? "Toi" : "NUMA"}</span>
              <span className="message-time">{formatTime(msg.timestamp)}</span>
            </div>
            <div className={`message-bubble ${msg.role === "user" ? "bubble-user" : "bubble-numa"} ${msg.category ? `bubble-${msg.category}` : ""}`}>
              {renderText(msg.text)}
            </div>
          </div>
        </div>
      ))}
      {isTyping && (
        <div className="message-row message-numa">
          <div className="message-avatar avatar-numa">N</div>
          <div className="message-content">
            <div className="typing-indicator">
              <span /><span /><span />
            </div>
          </div>
        </div>
      )}
      <div ref={endRef} />
    </div>
  );
}
