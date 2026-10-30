import { useState, useRef, useEffect, useCallback } from "react";
import type { Message } from "../types";
import { numaRespond } from "../lib/numa";
import { suggestions } from "../lib/knowledge";
import ChatHeader from "./ChatHeader";
import MessageList from "./MessageList";
import ChatInput from "./ChatInput";

const STORAGE_KEY = "numa-messages";
const NAME_KEY = "numa-username";

function loadMessages(): Message[] {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (!raw) return [];
    const parsed = JSON.parse(raw);
    if (!Array.isArray(parsed)) return [];
    return parsed;
  } catch {
    return [];
  }
}

function loadName(): string {
  return localStorage.getItem(NAME_KEY) ?? "";
}

export default function App() {
  const [messages, setMessages] = useState<Message[]>(loadMessages);
  const [savedName, setSavedName] = useState<string>(loadName);
  const [isTyping, setIsTyping] = useState(false);
  const scrollRef = useRef<HTMLDivElement>(null);
  const typingTimer = useRef<ReturnType<typeof setTimeout> | null>(null);

  useEffect(() => {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(messages));
  }, [messages]);

  useEffect(() => {
    localStorage.setItem(NAME_KEY, savedName);
  }, [savedName]);

  useEffect(() => {
    scrollRef.current?.scrollTo({ top: scrollRef.current.scrollHeight, behavior: "smooth" });
  }, [messages, isTyping]);

  const sendMessage = useCallback(
    (text: string) => {
      const trimmed = text.trim();
      if (!trimmed) return;

      const userMsg: Message = {
        id: crypto.randomUUID(),
        role: "user",
        text: trimmed,
        timestamp: Date.now(),
      };
      setMessages((prev) => [...prev, userMsg]);
      setIsTyping(true);

      if (typingTimer.current) clearTimeout(typingTimer.current);

      typingTimer.current = setTimeout(() => {
        const { reply, newName } = numaRespond(trimmed, savedName);
        if (newName) setSavedName(newName);
        const numaMsg: Message = {
          id: crypto.randomUUID(),
          role: "numa",
          text: reply.text,
          category: reply.category,
          timestamp: Date.now(),
        };
        setMessages((prev) => [...prev, numaMsg]);
        setIsTyping(false);
      }, 450);
    },
    [savedName]
  );

  const clearChat = useCallback(() => {
    setMessages([]);
    setSavedName("");
  }, []);

  return (
    <div className="app-container">
      <ChatHeader messageCount={messages.length} onClear={clearChat} />
      <div className="chat-body" ref={scrollRef}>
        {messages.length === 0 ? (
          <WelcomeScreen onSuggestionClick={sendMessage} />
        ) : (
          <MessageList messages={messages} isTyping={isTyping} />
        )}
      </div>
      <ChatInput suggestions={suggestions} onSend={sendMessage} />
    </div>
  );
}

function WelcomeScreen({ onSuggestionClick }: { onSuggestionClick: (text: string) => void }) {
  const quickPicks = ["aide", "dérivées", "pythagore", "25 * 4 + 18", "constantes"];
  return (
    <div className="welcome">
      <div className="welcome-logo">N</div>
      <h1 className="welcome-title">NUMA</h1>
      <p className="welcome-subtitle">
        Ton assistant scientifique hors-ligne.
        <br />
        Pose une question en maths, physique ou chimie — ou entre un calcul.
      </p>
      <div className="welcome-suggestions">
        {quickPicks.map((s) => (
          <button
            key={s}
            className="suggestion-chip"
            onClick={() => onSuggestionClick(s)}
          >
            {s}
          </button>
        ))}
      </div>
    </div>
  );
}
